# Resolve MultiProcessShell (mps::host / mps::client).
# Default: find_package. Embed sibling sources when VOLITION_DEV_EMBED_MPS=ON.

set(VOLITION_MPS_SOURCE_DIR "" CACHE PATH "Path to MultiProcessShell when VOLITION_DEV_EMBED_MPS=ON")

set(_volition_have_mps FALSE)
set(VOLITION_MPS_VIA_SOURCE FALSE)

if(TARGET mps::host AND TARGET mps::client)
  set(_volition_have_mps TRUE)
else()
  find_package(MultiProcessShell CONFIG QUIET)
  if(TARGET mps::host AND TARGET mps::client)
    set(_volition_have_mps TRUE)
    message(STATUS "Volition: using installed MultiProcessShell")
  endif()
endif()

if(NOT _volition_have_mps AND VOLITION_DEV_EMBED_MPS)
  if(VOLITION_MPS_SOURCE_DIR STREQUAL "" AND EXISTS "${CMAKE_SOURCE_DIR}/../MultiProcessShell/CMakeLists.txt")
    set(VOLITION_MPS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../MultiProcessShell")
  endif()
  if(VOLITION_MPS_SOURCE_DIR AND EXISTS "${VOLITION_MPS_SOURCE_DIR}/CMakeLists.txt")
    set(MPS_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
    set(MPS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(MPS_INSTALL OFF CACHE BOOL "" FORCE)
    set(MPS_DEV_EMBED_QTE OFF CACHE BOOL "" FORCE)
    set(MPS_DEV_EMBED_QFR OFF CACHE BOOL "" FORCE)
    add_subdirectory("${VOLITION_MPS_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/_deps/mps" EXCLUDE_FROM_ALL)
    set(_volition_have_mps TRUE)
    set(VOLITION_MPS_VIA_SOURCE TRUE)
    message(STATUS "Volition: DEV embed MultiProcessShell from ${VOLITION_MPS_SOURCE_DIR}")
  endif()
endif()

if(NOT _volition_have_mps)
  message(FATAL_ERROR
    "MultiProcessShell not found (required for volition_host / clients).\n"
    "  Install MPS and pass -DCMAKE_PREFIX_PATH=<prefix>;<qt>\n"
    "  Or: -DVOLITION_DEV_EMBED_MPS=ON [-DVOLITION_MPS_SOURCE_DIR=...]")
endif()
