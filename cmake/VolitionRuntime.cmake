# Copy shared runtime DLLs beside Host (Clients share the same bin/ directory).

function(volition_copy_runtime_deps target_name)
  if(NOT TARGET ${target_name})
    return()
  endif()

  set(_cmds "")
  foreach(_t IN ITEMS mps_ipc mps_ipc_qt mps_host mps_client)
    if(TARGET ${_t})
      list(APPEND _cmds
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${_t}>
                $<TARGET_FILE_DIR:${target_name}>
      )
    endif()
  endforeach()
  if(TARGET ${VOLITION_QTE_TARGET})
    list(APPEND _cmds
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              $<TARGET_FILE:${VOLITION_QTE_TARGET}>
              $<TARGET_FILE_DIR:${target_name}>
    )
  endif()
  if(TARGET ${VOLITION_QFR_TARGET})
    list(APPEND _cmds
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              $<TARGET_FILE:${VOLITION_QFR_TARGET}>
              $<TARGET_FILE_DIR:${target_name}>
    )
  endif()
  if(DEFINED MPS_PROTOBUF_RUNTIME_TARGETS)
    foreach(_t IN LISTS MPS_PROTOBUF_RUNTIME_TARGETS)
      if(TARGET ${_t})
        list(APPEND _cmds
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
                  $<TARGET_FILE:${_t}>
                  $<TARGET_FILE_DIR:${target_name}>
        )
      endif()
    endforeach()
  endif()

  if(_cmds)
    add_custom_command(TARGET ${target_name} POST_BUILD
      ${_cmds}
      COMMENT "Copy MPS/QTE/QFR/protobuf runtime beside ${target_name}"
      VERBATIM
    )
  endif()
endfunction()
