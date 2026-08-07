# volition_pdf — thin exe + SHARED business DLL.

add_library(volition_pdf_lib SHARED
  plugin.cpp
  pdf_document_view.hpp
)

target_compile_definitions(volition_pdf_lib PRIVATE VOLITION_CLIENT_LIB_BUILD)

# P0 placeholder UI only: do not link pdfium yet — pdfium's abseil_dll.dll clashes with
# FetchContent protobuf's abseil_dll.dll in the same process. Wire pdfium when rendering
# lands (private DLL dir / matching Abseil / delay-load).
target_link_libraries(volition_pdf_lib PRIVATE
  volition_client_ui
)

set_target_properties(volition_pdf_lib PROPERTIES
  OUTPUT_NAME volition_pdf
  AUTOMOC ON
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

volition_add_thin_client_exe(volition_pdf volition_pdf_lib "volition_pdf")

set_target_properties(volition_pdf PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

volition_copy_runtime_deps(volition_pdf_lib)
