# volition_host — MPS ShellWindow + multi-kind Client launchers.

add_executable(volition_host
  main.cpp
  home_content.cpp
  home_content.hpp
  document_open_service.cpp
  document_open_service.hpp
  client_kind.hpp
  theme_service.cpp
  theme_service.hpp
)

target_link_libraries(volition_host PRIVATE
  mps::host
  ${VOLITION_QTE_TARGET}
  Qt6::Widgets
)

set_target_properties(volition_host PROPERTIES
  AUTOMOC ON
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(WIN32)
  set_target_properties(volition_host PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

volition_copy_runtime_deps(volition_host)
