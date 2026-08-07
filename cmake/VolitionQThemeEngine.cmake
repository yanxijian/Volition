# Resolve QThemeEngine for Host / Client theming.

set(VOLITION_QTE_SOURCE_DIR "" CACHE PATH "Path to QThemeEngine when VOLITION_DEV_EMBED_QTE=ON")

set(_volition_have_qte FALSE)
set(VOLITION_QTE_VIA_SOURCE FALSE)

if(TARGET QThemeEngine::engine OR TARGET qte_engine)
  set(_volition_have_qte TRUE)
else()
  find_package(QThemeEngine CONFIG QUIET)
  if(TARGET QThemeEngine::engine)
    set(_volition_have_qte TRUE)
    message(STATUS "Volition: using installed QThemeEngine::engine")
  endif()
endif()

if(NOT _volition_have_qte AND VOLITION_DEV_EMBED_QTE)
  if(VOLITION_QTE_SOURCE_DIR STREQUAL "" AND EXISTS "${CMAKE_SOURCE_DIR}/../QThemeEngine/CMakeLists.txt")
    set(VOLITION_QTE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../QThemeEngine")
  endif()
  if(VOLITION_QTE_SOURCE_DIR AND EXISTS "${VOLITION_QTE_SOURCE_DIR}/CMakeLists.txt")
    set(QTE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(QTE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(QTE_BUILD_WIDGETS OFF CACHE BOOL "" FORCE)
    set(QTE_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory("${VOLITION_QTE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/_deps/qte" EXCLUDE_FROM_ALL)
    set(_volition_have_qte TRUE)
    set(VOLITION_QTE_VIA_SOURCE TRUE)
    message(STATUS "Volition: DEV embed QThemeEngine from ${VOLITION_QTE_SOURCE_DIR}")
  endif()
endif()

if(NOT _volition_have_qte)
  message(FATAL_ERROR
    "QThemeEngine not found.\n"
    "  Install QTE and pass -DCMAKE_PREFIX_PATH=<prefix>;<qt>\n"
    "  Or: -DVOLITION_DEV_EMBED_QTE=ON [-DVOLITION_QTE_SOURCE_DIR=...]")
endif()

if(TARGET QThemeEngine::engine)
  set(VOLITION_QTE_TARGET QThemeEngine::engine)
else()
  set(VOLITION_QTE_TARGET qte_engine)
endif()
