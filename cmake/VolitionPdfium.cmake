# Resolve pdfium from pdfium_all.
# Prefer staged prefix (pdfium_all/output): include/pdfium/public + lib/pdfium + bin/pdfium.dll.
# Optional: find_package(pdfium) or VOLITION_DEV_EMBED_PDFIUM=add_subdirectory (needs vcpkg + Clang-cl).

set(VOLITION_PDFIUM_PREFIX "" CACHE PATH
  "pdfium_all stage prefix (…/output) with include/ lib/ bin/")
set(VOLITION_PDFIUM_SOURCE_DIR "" CACHE PATH
  "Path to pdfium sources when VOLITION_DEV_EMBED_PDFIUM=ON")

set(_volition_have_pdfium FALSE)
set(VOLITION_PDFIUM_VIA_SOURCE FALSE)
set(VOLITION_PDFIUM_VIA_PREFIX FALSE)

if(TARGET pdfium::pdfium OR TARGET pdfium)
  set(_volition_have_pdfium TRUE)
endif()

# --- staged pdfium_all/output ---
if(NOT _volition_have_pdfium)
  set(_volition_prefix_candidates "")
  if(NOT VOLITION_PDFIUM_PREFIX STREQUAL "")
    list(APPEND _volition_prefix_candidates "${VOLITION_PDFIUM_PREFIX}")
  endif()
  list(APPEND _volition_prefix_candidates
    "${CMAKE_SOURCE_DIR}/../pdfium_all/output"
  )
  foreach(_pref IN LISTS _volition_prefix_candidates)
    set(_inc "${_pref}/include/pdfium")
    set(_lib "${_pref}/lib")
    if(EXISTS "${_inc}/public/fpdfview.h")
      find_library(VOLITION_PDFIUM_LIB
        NAMES pdfium
        PATHS "${_lib}"
        NO_DEFAULT_PATH
      )
      if(VOLITION_PDFIUM_LIB)
        set(VOLITION_PDFIUM_PREFIX "${_pref}" CACHE PATH "" FORCE)
        add_library(volition_pdfium SHARED IMPORTED GLOBAL)
        set_target_properties(volition_pdfium PROPERTIES
          IMPORTED_LOCATION "${_pref}/bin/pdfium${CMAKE_SHARED_LIBRARY_SUFFIX}"
          IMPORTED_IMPLIB "${VOLITION_PDFIUM_LIB}"
          INTERFACE_INCLUDE_DIRECTORIES "${_inc}"
        )
        # Staged headers live under include/pdfium/; samples use "public/fpdfview.h".
        add_library(pdfium::pdfium ALIAS volition_pdfium)
        set(_volition_have_pdfium TRUE)
        set(VOLITION_PDFIUM_VIA_PREFIX TRUE)
        message(STATUS "Volition: using staged pdfium_all from ${VOLITION_PDFIUM_PREFIX}")
        break()
      endif()
    endif()
  endforeach()
endif()

if(NOT _volition_have_pdfium)
  find_package(pdfium CONFIG QUIET)
  if(TARGET pdfium::pdfium)
    set(_volition_have_pdfium TRUE)
    message(STATUS "Volition: using installed pdfium::pdfium")
  endif()
endif()

if(NOT _volition_have_pdfium AND VOLITION_DEV_EMBED_PDFIUM)
  set(_volition_pdfium_candidates "")
  if(NOT VOLITION_PDFIUM_SOURCE_DIR STREQUAL "")
    list(APPEND _volition_pdfium_candidates "${VOLITION_PDFIUM_SOURCE_DIR}")
  endif()
  list(APPEND _volition_pdfium_candidates
    "${CMAKE_SOURCE_DIR}/../pdfium_all/pdfium"
    "${CMAKE_SOURCE_DIR}/../pdfium_all"
  )
  foreach(_cand IN LISTS _volition_pdfium_candidates)
    if(EXISTS "${_cand}/CMakeLists.txt" AND EXISTS "${_cand}/public/fpdfview.h")
      set(VOLITION_PDFIUM_SOURCE_DIR "${_cand}")
      break()
    elseif(EXISTS "${_cand}/pdfium/CMakeLists.txt" AND EXISTS "${_cand}/pdfium/public/fpdfview.h")
      set(VOLITION_PDFIUM_SOURCE_DIR "${_cand}/pdfium")
      break()
    endif()
  endforeach()

  if(VOLITION_PDFIUM_SOURCE_DIR AND EXISTS "${VOLITION_PDFIUM_SOURCE_DIR}/CMakeLists.txt")
    set(PDFIUM_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory("${VOLITION_PDFIUM_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/_deps/pdfium" EXCLUDE_FROM_ALL)
    set(_volition_have_pdfium TRUE)
    set(VOLITION_PDFIUM_VIA_SOURCE TRUE)
    message(STATUS "Volition: DEV embed pdfium from ${VOLITION_PDFIUM_SOURCE_DIR}")
  endif()
endif()

if(NOT _volition_have_pdfium)
  option(VOLITION_REQUIRE_PDFIUM
    "Fail configure when pdfium is missing" OFF)
  if(VOLITION_REQUIRE_PDFIUM)
    message(FATAL_ERROR
      "pdfium not found (VOLITION_REQUIRE_PDFIUM=ON).\n"
      "  Stage with pdfium_all scripts → ../pdfium_all/output (include/ lib/ bin/)\n"
      "  Or: -DVOLITION_PDFIUM_PREFIX=<path-to-output>\n"
      "  Or: find_package / -DVOLITION_DEV_EMBED_PDFIUM=ON (needs vcpkg + Clang-cl)")
  else()
    message(STATUS
      "Volition: pdfium not found — skipping (set VOLITION_REQUIRE_PDFIUM=ON to enforce)")
    set(VOLITION_PDFIUM_TARGET "")
  endif()
elseif(TARGET pdfium::pdfium)
  set(VOLITION_PDFIUM_TARGET pdfium::pdfium)
elseif(TARGET volition_pdfium)
  set(VOLITION_PDFIUM_TARGET volition_pdfium)
elseif(TARGET pdfium)
  set(VOLITION_PDFIUM_TARGET pdfium)
else()
  set(VOLITION_PDFIUM_TARGET "")
  message(STATUS "Volition: pdfium target missing after discovery")
endif()
