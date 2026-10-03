# Copy shared runtime DLLs beside Host (Clients share the same bin/ directory).

function(volition_copy_runtime_deps target_name)
  if(NOT TARGET ${target_name})
    return()
  endif()

  set(_cmds "")
  set(_dep_targets "")
  foreach(_t IN ITEMS mps_ipc mps_ipc_qt mps_host mps_client)
    if(TARGET ${_t})
      list(APPEND _cmds
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${_t}>
                $<TARGET_FILE_DIR:${target_name}>
      )
      list(APPEND _dep_targets ${_t})
    endif()
  endforeach()
  if(TARGET ${VOLITION_QTE_TARGET})
    list(APPEND _cmds
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              $<TARGET_FILE:${VOLITION_QTE_TARGET}>
              $<TARGET_FILE_DIR:${target_name}>
    )
    list(APPEND _dep_targets ${VOLITION_QTE_TARGET})
  endif()
  if(TARGET ${VOLITION_QFR_TARGET})
    list(APPEND _cmds
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              $<TARGET_FILE:${VOLITION_QFR_TARGET}>
              $<TARGET_FILE_DIR:${target_name}>
    )
    list(APPEND _dep_targets ${VOLITION_QFR_TARGET})
  endif()
  if(DEFINED MPS_PROTOBUF_RUNTIME_TARGETS)
    foreach(_t IN LISTS MPS_PROTOBUF_RUNTIME_TARGETS)
      if(TARGET ${_t})
        list(APPEND _cmds
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
                  $<TARGET_FILE:${_t}>
                  $<TARGET_FILE_DIR:${target_name}>
        )
        list(APPEND _dep_targets ${_t})
      endif()
    endforeach()
  endif()

  if(_cmds)
    # Braces: POST_BUILD on ${target_name} alone never fires when only a
    # dependency DLL relinks (unchanged export surface → exe does not
    # relink), which left a stale copy in bin/ and cost a full debug
    # round-trip. Drive a copy target from the DLL targets themselves and
    # make ${target_name} depend on it (no cycle: copy target depends only
    # on the runtime DLL targets, not on ${target_name}).
    add_custom_target(${target_name}_runtime_deps
      COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${target_name}>
      ${_cmds}
      DEPENDS ${_dep_targets}
      COMMENT "Copy MPS/QTE/QFR/protobuf runtime beside ${target_name}"
      VERBATIM
    )
    add_dependencies(${target_name} ${target_name}_runtime_deps)
  endif()
endfunction()
