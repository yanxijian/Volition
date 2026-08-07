# volition_markdown — thin exe + SHARED business DLL.

include(FetchContent)
FetchContent_Declare(md4c
  GIT_REPOSITORY https://github.com/mity/md4c.git
  GIT_TAG release-0.5.2
  GIT_SHALLOW TRUE
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_MD2HTML_EXECUTABLE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(md4c)

add_library(volition_markdown_lib SHARED
  plugin.cpp
  markdown_document_view.cpp
  markdown_document_view.hpp
)

target_compile_definitions(volition_markdown_lib PRIVATE VOLITION_CLIENT_LIB_BUILD)

target_link_libraries(volition_markdown_lib PRIVATE
  volition_client_ui
  md4c-html
)

target_include_directories(volition_markdown_lib PRIVATE
  "${md4c_SOURCE_DIR}/src"
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
