# Shared Client workspace UI (static) — linked into each volition_*_lib.

add_library(volition_client_ui STATIC
  document_stack.cpp
  document_stack.hpp
  document_view.hpp
  navigation_pane.hpp
  utility_pane.hpp
  workspace_layout.cpp
  workspace_layout.hpp
  workspace_window.cpp
  workspace_window.hpp
  workspace_content_view.cpp
  workspace_content_view.hpp
  client_run_helper.hpp
  volition_client_export.hpp
  volition_client_loader.hpp
)

target_include_directories(volition_client_ui
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(volition_client_ui
  PUBLIC
    mps::client
    ${VOLITION_QFR_TARGET}
    ${VOLITION_QTE_TARGET}
    Qt6::Widgets
)

set_target_properties(volition_client_ui PROPERTIES
  AUTOMOC ON
  POSITION_INDEPENDENT_CODE ON
)
