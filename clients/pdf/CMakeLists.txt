# volition_pdf — thin exe + SHARED business DLL with in-process pdfium (AbseilPin).
# Optional OOP helper: -DVOLITION_PDF_OOP_RENDER=ON (+ env VOLITION_PDF_OOP=1 at runtime).

set(_volition_pdf_outdir "${CMAKE_BINARY_DIR}/bin")
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

# Shared MPS/QTE/QFR/protobuf runtimes: staged once beside volition_host (same bin/).
# Pin abseil is copied with the Host; pdfium product runtimes stay on this target.

if(VOLITION_PDFIUM_TARGET AND NOT VOLITION_PDFIUM_TARGET STREQUAL "")
  if(TARGET volition_pdfium)
    target_link_libraries(volition_pdf_lib PRIVATE volition_pdfium)
    target_include_directories(volition_pdf_lib PRIVATE "${VOLITION_PDFIUM_PREFIX}/include/pdfium")
  elseif(TARGET ${VOLITION_PDFIUM_TARGET})
    target_link_libraries(volition_pdf_lib PRIVATE ${VOLITION_PDFIUM_TARGET})
  endif()
  volition_copy_pdfium_runtime(volition_pdf_lib)
  # Exclude V8 / alternate-abseil runtimes from the product bin/ layout.
  add_custom_command(TARGET volition_pdf_lib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -f
            "$<TARGET_FILE_DIR:volition_pdf_lib>/v8.dll"
            "$<TARGET_FILE_DIR:volition_pdf_lib>/v8_libbase.dll"
            "$<TARGET_FILE_DIR:volition_pdf_lib>/v8_libplatform.dll"
            "$<TARGET_FILE_DIR:volition_pdf_lib>/libc++.dll"
            "$<TARGET_FILE_DIR:volition_pdf_lib>/third_party_abseil-cpp_absl.dll"
            "$<TARGET_FILE_DIR:volition_pdf_lib>/simple_no_v8.exe"
            "$<TARGET_FILE_DIR:volition_pdf_lib>/simple_with_v8.exe"
    COMMENT "Exclude non-product pdfium runtimes from bin/"
    VERBATIM
  )
  message(STATUS "Volition: pdfium linked in-process into volition_pdf")
else()
  message(STATUS "Volition: pdfium not found — PDF Client will fail to open documents")
endif()

if(VOLITION_PDF_OOP_RENDER AND VOLITION_PDFIUM_TARGET AND NOT VOLITION_PDFIUM_TARGET STREQUAL "")
  set(_volition_pdf_render_outdir "${_volition_pdf_outdir}/render")
  file(MAKE_DIRECTORY "${_volition_pdf_render_outdir}")
  target_compile_definitions(volition_pdf_lib PRIVATE VOLITION_PDF_OOP_RENDER)
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
      COMMENT "Copy pdfium runtime into bin/render"
      VERBATIM
    )
  endif()
  message(STATUS "Volition: VOLITION_PDF_OOP_RENDER=ON (runtime VOLITION_PDF_OOP=1)")
endif()
