# Resolve QFluentRibbon for Client WorkspaceWindow (RibbonWindow).

set(VOLITION_QFR_SOURCE_DIR "" CACHE PATH "Path to QFluentRibbon when VOLITION_DEV_EMBED_QFR=ON")

set(_volition_have_qfr FALSE)
set(VOLITION_QFR_VIA_SOURCE FALSE)

if(TARGET QFluentRibbon::ribbon OR TARGET qfr_ribbon)
  set(_volition_have_qfr TRUE)
else()
  find_package(QFluentRibbon CONFIG QUIET)
  if(TARGET QFluentRibbon::ribbon)
    set(_volition_have_qfr TRUE)
    message(STATUS "Volition: using installed QFluentRibbon::ribbon")
  endif()
endif()

if(NOT _volition_have_qfr AND VOLITION_DEV_EMBED_QFR)
  if(VOLITION_QFR_SOURCE_DIR STREQUAL "" AND EXISTS "${CMAKE_SOURCE_DIR}/../QFluentRibbon/CMakeLists.txt")
    set(VOLITION_QFR_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../QFluentRibbon")
  endif()
  if(VOLITION_QFR_SOURCE_DIR AND EXISTS "${VOLITION_QFR_SOURCE_DIR}/CMakeLists.txt")
    set(QFR_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(QFR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(QFR_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory("${VOLITION_QFR_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/_deps/qfr" EXCLUDE_FROM_ALL)
    set(_volition_have_qfr TRUE)
    set(VOLITION_QFR_VIA_SOURCE TRUE)
    message(STATUS "Volition: DEV embed QFluentRibbon from ${VOLITION_QFR_SOURCE_DIR}")
  endif()
endif()

if(NOT _volition_have_qfr)
  message(FATAL_ERROR
    "QFluentRibbon not found (required for Client WorkspaceWindow).\n"
    "  Install QFR and pass -DCMAKE_PREFIX_PATH=<prefix>;<qt>\n"
    "  Or: -DVOLITION_DEV_EMBED_QFR=ON [-DVOLITION_QFR_SOURCE_DIR=...]")
endif()

if(TARGET QFluentRibbon::ribbon)
  set(VOLITION_QFR_TARGET QFluentRibbon::ribbon)
else()
  set(VOLITION_QFR_TARGET qfr_ribbon)
endif()
