#!/usr/bin/env python3
"""Format handwritten C/C++ sources with clang-format.

Excludes: build/, _deps/, *.pb.* (see .clang-format-ignore).
After in-place format, restores UTF-8 BOM (clang-format drops it).
Also checks ColumnLimit-friendly formatting via clang-format --dry-run.

Usage (from repo root):
  python scripts/format_source.py
  python scripts/format_source.py --check
"""

from __future__ import annotations

import argparse
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXTS = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx"}
BOM = b"\xef\xbb\xbf"


def find_clang_format() -> Path | None:
	env = os.environ.get("CLANG_FORMAT")
	if env:
		p = Path(env)
		if p.is_file():
			return p
	for candidate in (
		Path(r"C:\Program Files\LLVM\bin\clang-format.exe"),
		Path(r"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\Llvm\x64\bin\clang-format.exe"),
	):
		if candidate.is_file():
			return candidate
	from shutil import which

	w = which("clang-format")
	return Path(w) if w else None


def is_excluded(rel: Path) -> bool:
	parts = {p.lower() for p in rel.parts}
	if "build" in parts or "_deps" in parts:
		return True
	if any(p.endswith("_autogen") for p in parts):
		return True
	name = rel.name.lower()
	if ".pb." in name or name.endswith(".pb.cc") or name.endswith(".pb.h"):
		return True
	return False


def collect_sources() -> list[Path]:
	out: list[Path] = []
	for path in ROOT.rglob("*"):
		if not path.is_file() or path.suffix.lower() not in EXTS:
			continue
		try:
			rel = path.relative_to(ROOT)
		except ValueError:
			continue
		if is_excluded(rel):
			continue
		out.append(path)
	return sorted(out)


def ensure_utf8_bom_crlf(path: Path) -> None:
	"""Normalize to UTF-8 BOM + CRLF (cpp-source-encoding.mdc)."""
	raw = path.read_bytes()
	if raw.startswith(BOM):
		body = raw[len(BOM) :]
	else:
		body = raw
	text = body.decode("utf-8")
	text = text.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n")
	path.write_bytes(BOM + text.encode("utf-8"))


def check_encoding(path: Path) -> list[str]:
	"""Return list of encoding/EOL problems for --check."""
	problems: list[str] = []
	raw = path.read_bytes()
	if not raw.startswith(BOM):
		problems.append("missing UTF-8 BOM")
	body = raw[len(BOM) :] if raw.startswith(BOM) else raw
	try:
		text = body.decode("utf-8")
	except UnicodeDecodeError:
		problems.append("not valid UTF-8")
		return problems
	if "\r\n" not in text and "\n" in text:
		problems.append("LF-only line endings (want CRLF)")
	elif "\r\n" in text:
		# Detect mixed: lone LF remaining after stripping CRLF pairs
		stripped = text.replace("\r\n", "")
		if "\n" in stripped or "\r" in stripped:
			problems.append("mixed line endings")
	return problems


def visual_width(line: str, tab_width: int = 4) -> int:
	w = 0
	for ch in line.rstrip("\r\n"):
		if ch == "\t":
			w = (w // tab_width + 1) * tab_width
		else:
			w += 1
	return w


def check_column_limit(path: Path, limit: int = 140) -> list[int]:
	"""Return 1-based line numbers exceeding ColumnLimit (tab width 4)."""
	raw = path.read_bytes()
	body = raw[len(BOM) :] if raw.startswith(BOM) else raw
	text = body.decode("utf-8")
	bad: list[int] = []
	for i, line in enumerate(text.splitlines(), start=1):
		if visual_width(line) > limit:
			bad.append(i)
	return bad


def main() -> int:
	ap = argparse.ArgumentParser(description=__doc__)
	ap.add_argument(
		"--check",
		action="store_true",
		help="Only check formatting/encoding (non-zero if changes needed)",
	)
	args = ap.parse_args()

	cf = find_clang_format()
	if not cf:
		print("error: clang-format not found; set CLANG_FORMAT or install LLVM", file=sys.stderr)
		return 2

	files = collect_sources()
	if not files:
		print("no sources found")
		return 0

	mode = ["--dry-run", "--Werror"] if args.check else ["-i"]
	batch = 40
	rc = 0
	for i in range(0, len(files), batch):
		chunk = files[i : i + batch]
		cmd = [str(cf), "-style=file", *mode, *[str(p) for p in chunk]]
		print(f"+ clang-format ({len(chunk)} files){' --check' if args.check else ''}", flush=True)
		r = subprocess.run(cmd, cwd=ROOT)
		if r.returncode != 0:
			rc = r.returncode
			if args.check:
				return rc

	if not args.check:
		for path in files:
			ensure_utf8_bom_crlf(path)
		print(f"+ restored UTF-8 BOM + CRLF on {len(files)} files", flush=True)
	else:
		enc_fail = 0
		col_fail = 0
		for path in files:
			rel = path.relative_to(ROOT)
			for prob in check_encoding(path):
				print(f"error: {rel}: {prob}", file=sys.stderr)
				enc_fail += 1
			for lineno in check_column_limit(path):
				print(f"error: {rel}:{lineno}: exceeds ColumnLimit 140", file=sys.stderr)
				col_fail += 1
		if enc_fail or col_fail:
			return 1

	print(f"OK: {'check passed' if args.check else 'formatted'} {len(files)} files")
	return rc


if __name__ == "__main__":
	sys.exit(main())
