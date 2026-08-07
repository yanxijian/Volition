# volition_text — thin exe + SHARED business DLL.

add_library(volition_text_lib SHARED
  plugin.cpp
  text_document_view.hpp
)

target_compile_definitions(volition_text_lib PRIVATE VOLITION_CLIENT_LIB_BUILD)

target_link_libraries(volition_text_lib PRIVATE
  volition_client_ui
)

set_target_properties(volition_text_lib PROPERTIES
  OUTPUT_NAME volition_text
  AUTOMOC ON
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

volition_add_thin_client_exe(volition_text volition_text_lib "volition_text")

set_target_properties(volition_text PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

volition_copy_runtime_deps(volition_text_lib)
