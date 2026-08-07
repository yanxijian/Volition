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

# Ensure AbseilPin abseil is beside Host when using the pin (same bin/ as Clients).
if(WIN32 AND MPS_ABSEIL_PIN_PREFIX AND NOT MPS_ABSEIL_PIN_PREFIX STREQUAL "")
  set(_volition_pin_absl "${MPS_ABSEIL_PIN_PREFIX}/bin/abseil_dll.dll")
  if(EXISTS "${_volition_pin_absl}")
    add_custom_command(TARGET volition_host POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              "${_volition_pin_absl}"
              "$<TARGET_FILE_DIR:volition_host>/abseil_dll.dll"
      COMMENT "Copy AbseilPin abseil_dll beside volition_host"
      VERBATIM
    )
  endif()
endif()
