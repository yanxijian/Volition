<#
.SYNOPSIS
  Smoke-check Volition build/bin layout (Abseil pin, PDF client, no nested pdf/).

.DESCRIPTION
  Validates the Stage-2 product layout after cmake --build:
  - Shared runtimes live under a single bin/ (no bin/pdf/)
  - At most one abseil_dll.dll; optionally must match AbseilPin hash
  - PDF client is flat beside Host; default path has no OOP render helper
  - When pdfium.dll is present, volition_pdf.dll depends on it and V8 runtimes are absent

.EXAMPLE
  .\scripts\smoke_bin_layout.ps1 -BinDir .\build\bin
  .\scripts\smoke_bin_layout.ps1 -BinDir .\build\bin -AbseilPinPrefix ..\AbseilPin\prefix\20260107.1 -RequirePdfium
#>
param(
  [string] $BinDir = "",
  [string] $AbseilPinPrefix = "",
  [switch] $RequirePdfium
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
if (-not $BinDir) {
  $BinDir = Join-Path $RepoRoot "build\bin"
}
$BinDir = (Resolve-Path -LiteralPath $BinDir).Path

function Fail([string] $Message) {
  Write-Error $Message
  exit 1
}

function Ok([string] $Message) {
  Write-Host "OK  $Message"
}

if (-not (Test-Path -LiteralPath $BinDir)) {
  Fail "BinDir not found: $BinDir"
}

Write-Host "smoke_bin_layout: $BinDir"

# --- layout ---
if (Test-Path -LiteralPath (Join-Path $BinDir "pdf")) {
  Fail "Obsolete nested Client dir present: $BinDir\pdf (expect flat bin/)"
}
Ok "no nested bin/pdf/"

foreach ($name in @("volition_host.exe", "volition_text.exe", "volition_markdown.exe", "volition_pdf.exe", "volition_pdf.dll")) {
  $p = Join-Path $BinDir $name
  if (-not (Test-Path -LiteralPath $p)) {
    Fail "Missing $name under $BinDir"
  }
}
Ok "Host + Client thin exes + volition_pdf.dll"

$renderExe = Join-Path $BinDir "render\volition_pdf_render.exe"
if (Test-Path -LiteralPath $renderExe) {
  Write-Host "NOTE OOP helper present (VOLITION_PDF_OOP_RENDER build): $renderExe"
} else {
  Ok "default path has no bin/render/volition_pdf_render.exe"
}

# --- single abseil ---
$abslFiles = @(Get-ChildItem -LiteralPath $BinDir -Recurse -Filter "abseil_dll.dll" -File -ErrorAction SilentlyContinue)
if ($abslFiles.Count -eq 0) {
  Fail "No abseil_dll.dll under $BinDir"
}
if ($abslFiles.Count -gt 1) {
  $list = ($abslFiles | ForEach-Object { $_.FullName }) -join "; "
  Fail "Expected one abseil_dll.dll under bin/, found $($abslFiles.Count): $list"
}
$absl = $abslFiles[0]
Ok "single abseil_dll.dll ($($absl.FullName))"

if (-not $AbseilPinPrefix) {
  $sibling = Join-Path (Split-Path $RepoRoot -Parent) "AbseilPin\prefix\20260107.1"
  if (Test-Path (Join-Path $sibling "bin\abseil_dll.dll")) {
    $AbseilPinPrefix = $sibling
  }
}
if ($AbseilPinPrefix) {
  $pinDll = Join-Path $AbseilPinPrefix "bin\abseil_dll.dll"
  if (-not (Test-Path -LiteralPath $pinDll)) {
    Fail "AbseilPinPrefix set but missing: $pinDll"
  }
  $hBin = (Get-FileHash -LiteralPath $absl.FullName -Algorithm SHA256).Hash
  $hPin = (Get-FileHash -LiteralPath $pinDll -Algorithm SHA256).Hash
  if ($hBin -ne $hPin) {
    Fail "abseil_dll.dll hash mismatch vs pin`n  bin=$hBin`n  pin=$hPin"
  }
  Ok "abseil_dll.dll matches AbseilPin ($AbseilPinPrefix)"
} else {
  Write-Host "NOTE AbseilPin prefix not found; skipped hash match"
}

# --- pdfium product path (optional) ---
$pdfium = Join-Path $BinDir "pdfium.dll"
$hasPdfium = Test-Path -LiteralPath $pdfium
if ($RequirePdfium -and -not $hasPdfium) {
  Fail "-RequirePdfium set but pdfium.dll missing under $BinDir"
}

if ($hasPdfium) {
  foreach ($bad in @("v8.dll", "v8_libbase.dll", "v8_libplatform.dll", "libc++.dll", "third_party_abseil-cpp_absl.dll")) {
    if (Test-Path -LiteralPath (Join-Path $BinDir $bad)) {
      Fail "Non-product runtime present beside Host: $bad"
    }
  }
  Ok "pdfium.dll present; no V8/libc++/third_party abseil in bin/"

  $dumpbin = Get-Command dumpbin.exe -ErrorAction SilentlyContinue
  if ($dumpbin) {
    $deps = & dumpbin.exe /dependents (Join-Path $BinDir "volition_pdf.dll") 2>$null | Out-String
    if ($deps -notmatch "(?i)pdfium\.dll") {
      Fail "volition_pdf.dll does not list pdfium.dll as a dependent"
    }
    Ok "volition_pdf.dll depends on pdfium.dll"
  } else {
    Write-Host "NOTE dumpbin not on PATH; skipped import check"
  }
} else {
  Write-Host "NOTE pdfium.dll absent; skipped in-process PDF checks (configure with staged pdfium_all/output to enable)"
}

Write-Host "smoke_bin_layout: PASSED"
exit 0
