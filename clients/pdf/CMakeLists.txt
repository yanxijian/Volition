# volition_pdf — thin exe + SHARED business DLL (MPS/Qt only).
# pdfium runs in sibling volition_pdf_render.exe under bin/pdf/render/ (own abseil).

set(_volition_pdf_outdir "${CMAKE_BINARY_DIR}/bin/pdf")
set(_volition_pdf_render_outdir "${_volition_pdf_outdir}/render")
file(MAKE_DIRECTORY "${_volition_pdf_outdir}")
file(MAKE_DIRECTORY "${_volition_pdf_render_outdir}")

add_library(volition_pdf_lib SHARED
  plugin.cpp
  pdf_document_view.cpp
  pdf_document_view.hpp
)

target_compile_definitions(volition_pdf_lib PRIVATE VOLITION_CLIENT_LIB_BUILD)

target_link_libraries(volition_pdf_lib PRIVATE
  volition_client_ui
)

set_target_properties(volition_pdf_lib PROPERTIES
  OUTPUT_NAME volition_pdf
  AUTOMOC ON
  RUNTIME_OUTPUT_DIRECTORY "${_volition_pdf_outdir}"
  LIBRARY_OUTPUT_DIRECTORY "${_volition_pdf_outdir}"
  ARCHIVE_OUTPUT_DIRECTORY "${_volition_pdf_outdir}"
)

volition_add_thin_client_exe(volition_pdf volition_pdf_lib "volition_pdf")

set_target_properties(volition_pdf PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${_volition_pdf_outdir}"
)

volition_copy_runtime_deps(volition_pdf_lib)

# Older builds copied pdfium into bin/pdf/ (abseil clash with MPS). Scrub leftovers after
# volition_copy_runtime_deps so Client keeps protobuf abseil, not pdfium's.
add_custom_command(TARGET volition_pdf_lib POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E rm -f
          "$<TARGET_FILE_DIR:volition_pdf_lib>/pdfium.dll"
          "$<TARGET_FILE_DIR:volition_pdf_lib>/v8.dll"
          "$<TARGET_FILE_DIR:volition_pdf_lib>/v8_libbase.dll"
          "$<TARGET_FILE_DIR:volition_pdf_lib>/v8_libplatform.dll"
          "$<TARGET_FILE_DIR:volition_pdf_lib>/third_party_abseil-cpp_absl.dll"
          "$<TARGET_FILE_DIR:volition_pdf_lib>/simple_no_v8.exe"
          "$<TARGET_FILE_DIR:volition_pdf_lib>/simple_with_v8.exe"
  COMMENT "Scrub in-process pdfium leftovers from bin/pdf"
  VERBATIM
)

# --- out-of-process pdfium helper (no MPS / no protobuf) ---
if(VOLITION_PDFIUM_TARGET AND NOT VOLITION_PDFIUM_TARGET STREQUAL "")
  add_executable(volition_pdf_render
    volition_pdf_render.cpp
  )
  if(TARGET volition_pdfium)
    target_link_libraries(volition_pdf_render PRIVATE volition_pdfium)
    target_include_directories(volition_pdf_render PRIVATE "${VOLITION_PDFIUM_PREFIX}/include/pdfium")
  elseif(TARGET ${VOLITION_PDFIUM_TARGET})
    target_link_libraries(volition_pdf_render PRIVATE ${VOLITION_PDFIUM_TARGET})
  endif()
  set_target_properties(volition_pdf_render PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${_volition_pdf_render_outdir}"
  )
  if(VOLITION_PDFIUM_VIA_PREFIX AND EXISTS "${VOLITION_PDFIUM_PREFIX}/bin")
    add_custom_command(TARGET volition_pdf_render POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
              "${VOLITION_PDFIUM_PREFIX}/bin"
              "${_volition_pdf_render_outdir}"
      COMMENT "Copy pdfium runtime into bin/pdf/render (isolated abseil)"
      VERBATIM
    )
  endif()
  add_dependencies(volition_pdf_lib volition_pdf_render)
  message(STATUS "Volition: pdfium via out-of-process volition_pdf_render")
else()
  message(STATUS "Volition: pdfium not found — PDF open will report missing render helper")
endif()
