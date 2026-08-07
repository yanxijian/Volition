# volition_pdf — thin exe + SHARED business DLL.
# Artifacts live under bin/pdf/ so pdfium's abseil does not overwrite Host protobuf abseil in bin/.

set(_volition_pdf_outdir "${CMAKE_BINARY_DIR}/bin/pdf")
file(MAKE_DIRECTORY "${_volition_pdf_outdir}")

add_library(volition_pdf_lib SHARED
  plugin.cpp
  pdf_document_view.cpp
  pdf_document_view.hpp
)

target_compile_definitions(volition_pdf_lib PRIVATE VOLITION_CLIENT_LIB_BUILD)

target_link_libraries(volition_pdf_lib PRIVATE
  volition_client_ui
)

if(VOLITION_PDFIUM_TARGET AND NOT VOLITION_PDFIUM_TARGET STREQUAL "")
  target_compile_definitions(volition_pdf_lib PRIVATE VOLITION_HAS_PDFIUM=1)
  if(TARGET volition_pdfium)
    target_link_libraries(volition_pdf_lib PRIVATE volition_pdfium)
    target_include_directories(volition_pdf_lib PRIVATE "${VOLITION_PDFIUM_PREFIX}/include/pdfium")
  elseif(TARGET ${VOLITION_PDFIUM_TARGET})
    target_link_libraries(volition_pdf_lib PRIVATE ${VOLITION_PDFIUM_TARGET})
  endif()
  message(STATUS "Volition: volition_pdf links pdfium (${VOLITION_PDFIUM_TARGET})")
else()
  message(STATUS "Volition: volition_pdf built without pdfium (placeholder open)")
endif()

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

# Copy pdfium runtime into bin/pdf/ only (includes abseil private to this Client).
if(VOLITION_PDFIUM_VIA_PREFIX AND EXISTS "${VOLITION_PDFIUM_PREFIX}/bin")
  add_custom_command(TARGET volition_pdf_lib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${VOLITION_PDFIUM_PREFIX}/bin"
            "${_volition_pdf_outdir}"
    COMMENT "Copy pdfium runtime into bin/pdf (isolated from Host abseil)"
    VERBATIM
  )
endif()
