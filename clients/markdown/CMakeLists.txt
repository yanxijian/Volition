# volition_markdown — thin exe + SHARED business DLL.

add_library(volition_markdown_lib SHARED
  plugin.cpp
  markdown_document_view.hpp
)

target_compile_definitions(volition_markdown_lib PRIVATE VOLITION_CLIENT_LIB_BUILD)

target_link_libraries(volition_markdown_lib PRIVATE
  volition_client_ui
)

set_target_properties(volition_markdown_lib PROPERTIES
  OUTPUT_NAME volition_markdown
  AUTOMOC ON
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

volition_add_thin_client_exe(volition_markdown volition_markdown_lib "volition_markdown")

set_target_properties(volition_markdown PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

volition_copy_runtime_deps(volition_markdown_lib)
