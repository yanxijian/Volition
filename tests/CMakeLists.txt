add_executable(volition_client_kind_test
  client_kind_test.cpp
)

target_link_libraries(volition_client_kind_test PRIVATE
  Qt6::Core
  Qt6::Test
)

target_include_directories(volition_client_kind_test PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/../host"
)

add_test(NAME volition_client_kind_test COMMAND volition_client_kind_test)