# volition_host — MPS ShellWindow + multi-kind Client launchers.

set(VOLITION_TS_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/../translations/volition_zh_CN.ts"
)

add_executable(volition_host
  main.cpp
  home_content.cpp
  home_content.hpp
  app_settings.hpp
  language_service.cpp
  language_service.hpp
  library_store.cpp
  library_store.hpp
  document_open_service.cpp
  document_open_service.hpp
  client_kind.hpp
  theme_service.cpp
  theme_service.hpp
)

find_package(Qt6 REQUIRED COMPONENTS LinguistTools)
set(VOLITION_QM_FILES "")
qt_add_lrelease(volition_host
  TS_FILES ${VOLITION_TS_FILES}
  QM_FILES_OUTPUT_VARIABLE VOLITION_QM_FILES
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

# Ship .qm next to the host: bin/langs/volition_*.qm
add_custom_command(TARGET volition_host POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:volition_host>/langs"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VOLITION_QM_FILES} "$<TARGET_FILE_DIR:volition_host>/langs/"
  COMMENT "Copy translation .qm files to bin/langs"
  VERBATIM
)

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
