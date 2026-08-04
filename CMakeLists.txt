cmake_minimum_required(VERSION 3.21)

project(Volition
  VERSION 0.1.0
  DESCRIPTION "Multi-format document reader/editor on MultiProcessShell"
  LANGUAGES CXX
)

# Scaffold only: Host / clients not implemented yet.
# Intended layout:
#   host/           → volition_host (MPS Host)
#   clients/text/   → volition_text (txt / xml / Markdown mode)
#   clients/pdf/    → volition_pdf
#
# Depends on sibling MultiProcessShell (and optionally QThemeEngine / QFluentRibbon).

message(STATUS "Volition ${PROJECT_VERSION}: scaffold — add targets when Host/Clients land")
