# Copy pdfium_all staged bin/ beside a target, skipping DLLs that clash with
# FetchContent protobuf (notably abseil_dll.dll — different ABI).

function(volition_copy_pdfium_runtime target_name)
  if(NOT VOLITION_PDFIUM_VIA_PREFIX)
    return()
  endif()
  if(NOT EXISTS "${VOLITION_PDFIUM_PREFIX}/bin")
    return()
  endif()
  if(NOT TARGET ${target_name})
    return()
  endif()

  # Skip names that collide with MPS protobuf / Qt / MSVC runtimes.
  set(_skip
    abseil_dll.dll
    abseil_dll.pdb
    libprotobuf.dll
    libprotoc.dll
    utf8_validity.dll
    Qt6Core.dll
    Qt6Gui.dll
    Qt6Widgets.dll
    Qt6Network.dll
  )

  set(_copy_cmds "")
  file(GLOB _pdfium_bin_files "${VOLITION_PDFIUM_PREFIX}/bin/*")
  foreach(_src IN LISTS _pdfium_bin_files)
    get_filename_component(_name "${_src}" NAME)
    list(FIND _skip "${_name}" _idx)
    if(_idx GREATER_EQUAL 0)
      continue()
    endif()
    # Skip pdb/exp/lib noise except keep dll/exe/dat
    get_filename_component(_ext "${_src}" EXT)
    string(TOLOWER "${_ext}" _ext_l)
    if(_ext_l STREQUAL ".pdb" OR _ext_l STREQUAL ".exp" OR _ext_l STREQUAL ".lib" OR _ext_l STREQUAL ".gitkeep")
      continue()
    endif()
    list(APPEND _copy_cmds
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              "${_src}"
              "$<TARGET_FILE_DIR:${target_name}>/${_name}"
    )
  endforeach()

  if(_copy_cmds)
    add_custom_command(TARGET ${target_name} POST_BUILD
      ${_copy_cmds}
      COMMENT "Copy pdfium runtime (skip protobuf abseil clash) beside ${target_name}"
      VERBATIM
    )
  endif()
endfunction()
