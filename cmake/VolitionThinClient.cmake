# Shared thin-exe helper: load volition_<kind>.dll and call VolitionClientRun.
# Usage from clients/<kind>/CMakeLists.txt after defining volition_<kind>_lib:
#   volition_add_thin_client_exe(volition_text volition_text_lib "volition_text")

function(volition_add_thin_client_exe exe_target lib_target library_base_name)
  add_executable(${exe_target}
    "${CMAKE_SOURCE_DIR}/clients/common/thin_main.cpp"
  )
  target_compile_definitions(${exe_target} PRIVATE
    VOLITION_CLIENT_DLL_BASENAME="${library_base_name}"
  )
  target_include_directories(${exe_target} PRIVATE
    "${CMAKE_SOURCE_DIR}/clients/common"
  )
  target_link_libraries(${exe_target} PRIVATE Qt6::Core)
  add_dependencies(${exe_target} ${lib_target})
  # Console subsystem: no WinMain; Host launches via QProcess.
  # No QApplication here — business DLL owns the Qt event loop.
endfunction()
