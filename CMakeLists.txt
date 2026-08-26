cmake_minimum_required(VERSION 3.21)

project(Volition
  VERSION 0.1.0
  DESCRIPTION "Multi-format document reader/editor on MultiProcessShell"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
  add_compile_options(/MP)
endif()

option(VOLITION_BUILD_APPS "Build volition_host and Client thin exe + DLLs" ON)
option(VOLITION_BUILD_TESTS "Build Volition unit tests" OFF)
option(VOLITION_DEV_EMBED_MPS "Embed sibling MultiProcessShell via add_subdirectory" ON)
option(VOLITION_DEV_EMBED_QTE "Embed sibling QThemeEngine via add_subdirectory" ON)
option(VOLITION_DEV_EMBED_QFR "Embed sibling QFluentRibbon via add_subdirectory" ON)
option(VOLITION_DEV_EMBED_PDFIUM
  "Embed pdfium sources via add_subdirectory (VolitionToolchain: MSVC cl + C++20 + V8 OFF; prefer staged output)" OFF)
option(VOLITION_PDF_OOP_RENDER
  "Build volition_pdf_render.exe and allow env VOLITION_PDF_OOP=1 fallback" OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(DEFINED ENV{QTDIR} AND NOT "$ENV{QTDIR}" STREQUAL "")
  file(TO_CMAKE_PATH "$ENV{QTDIR}" _volition_qtdir)
  list(PREPEND CMAKE_PREFIX_PATH "${_volition_qtdir}")
endif()

if(NOT VOLITION_BUILD_APPS)
  message(STATUS "Volition ${PROJECT_VERSION}: VOLITION_BUILD_APPS=OFF — scaffold only")
  return()
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
if(Qt6_VERSION VERSION_LESS "6.8")
  message(FATAL_ERROR "Qt 6.8+ required, found ${Qt6_VERSION}")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Do NOT set global CMAKE_RUNTIME_OUTPUT_DIRECTORY: FetchContent protobuf /
# abseil would land in bin/ and collide with staged pdfium runtimes.
# Volition targets set RUNTIME_OUTPUT_DIRECTORY individually; runtime DLLs are
# copied via volition_copy_runtime_deps / volition_copy_pdfium_runtime.

include(VolitionMultiProcessShell)
include(VolitionQThemeEngine)
include(VolitionQFluentRibbon)
include(VolitionPdfium)
include(VolitionThinClient)
include(VolitionRuntime)
include(VolitionPdfiumRuntime)

add_subdirectory(clients/common)
add_subdirectory(host)
add_subdirectory(clients/text)
add_subdirectory(clients/markdown)
set(_volition_have_pdf_client FALSE)
if(VOLITION_PDFIUM_TARGET AND NOT VOLITION_PDFIUM_TARGET STREQUAL "")
  add_subdirectory(clients/pdf)
  set(_volition_have_pdf_client TRUE)
else()
  message(STATUS "Volition: skipping clients/pdf (no pdfium; CI may omit staged prefix)")
endif()

if(VOLITION_BUILD_TESTS)
  enable_testing()
  find_package(Qt6 REQUIRED COMPONENTS Test)
  add_subdirectory(tests)
endif()

# Stage thin exes + business DLLs beside host (runtime DLLs also copied on each link via volition_copy_runtime_deps).
set(_volition_stage_cmds
  COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:volition_host>
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_FILE:volition_text>
          $<TARGET_FILE:volition_text_lib>
          $<TARGET_FILE_DIR:volition_host>
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_FILE:volition_markdown>
          $<TARGET_FILE:volition_markdown_lib>
          $<TARGET_FILE_DIR:volition_host>
)
set(_volition_stage_deps
  volition_host
  volition_text volition_text_lib
  volition_markdown volition_markdown_lib
)
if(_volition_have_pdf_client)
  list(APPEND _volition_stage_cmds
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:volition_pdf>
            $<TARGET_FILE:volition_pdf_lib>
            $<TARGET_FILE_DIR:volition_host>
  )
  list(APPEND _volition_stage_deps volition_pdf volition_pdf_lib)
endif()
if(VOLITION_PDF_OOP_RENDER AND TARGET volition_pdf_render)
  list(APPEND _volition_stage_cmds
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:volition_host>/render
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:volition_pdf_render>
            $<TARGET_FILE_DIR:volition_host>/render
  )
  list(APPEND _volition_stage_deps volition_pdf_render)
endif()

add_custom_target(volition_stage_clients ALL
  ${_volition_stage_cmds}
  COMMENT "Copy Client thin exes + DLLs beside volition_host"
  DEPENDS ${_volition_stage_deps}
  VERBATIM
)

# Qt runtime via windeployqt (host + Client thin exes share bin/).
if(NOT WINDEPLOYQT_EXECUTABLE)
  find_program(WINDEPLOYQT_EXECUTABLE
    NAMES windeployqt windeployqt.exe
    HINTS
      "$ENV{QTDIR}/bin"
      "${Qt6_DIR}/../../../bin"
    DOC "Qt windeployqt tool"
  )
endif()

if(WIN32 AND WINDEPLOYQT_EXECUTABLE)
  set(_volition_deploy_cmds
    COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            "$<IF:$<CONFIG:Debug>,--debug,--release>"
            --no-translations
            --compiler-runtime
            $<TARGET_FILE:volition_host>
    COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            "$<IF:$<CONFIG:Debug>,--debug,--release>"
            --no-translations
            $<TARGET_FILE:volition_text>
    COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            "$<IF:$<CONFIG:Debug>,--debug,--release>"
            --no-translations
            $<TARGET_FILE:volition_markdown>
  )
  if(_volition_have_pdf_client)
    list(APPEND _volition_deploy_cmds
      COMMAND "${WINDEPLOYQT_EXECUTABLE}"
              "$<IF:$<CONFIG:Debug>,--debug,--release>"
              --no-translations
              $<TARGET_FILE:volition_pdf>
    )
  endif()
  add_custom_target(volition_deploy_qt ALL
    ${_volition_deploy_cmds}
    COMMENT "windeployqt: deploy Qt runtime beside Volition binaries"
    DEPENDS volition_stage_clients
    VERBATIM
  )
elseif(WIN32)
  message(WARNING "windeployqt not found; set QTDIR or PATH — apps need Qt DLLs beside exe or on PATH")
endif()
message(STATUS "Volition ${PROJECT_VERSION}")
message(STATUS "  Qt: ${Qt6_VERSION}")
message(STATUS "  embed: mps=${VOLITION_MPS_VIA_SOURCE} qte=${VOLITION_QTE_VIA_SOURCE} qfr=${VOLITION_QFR_VIA_SOURCE} pdfium_src=${VOLITION_PDFIUM_VIA_SOURCE} pdfium_prefix=${VOLITION_PDFIUM_VIA_PREFIX}")
message(STATUS "  windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
