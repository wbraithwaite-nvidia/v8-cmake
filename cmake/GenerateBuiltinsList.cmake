function(v8_generate_builtins_list target-dir)
  set(directory ${target-dir}/builtins-generated)
  set(output ${directory}/bytecodes-builtins-list.h)
  add_custom_command(
    COMMAND ${CMAKE_COMMAND} -E make_directory ${directory}
    OUTPUT ${directory}
    COMMENT "Generating ${directory}"
    VERBATIM)
  add_custom_command(
    COMMAND bytecode_builtins_list_generator ${output}
    DEPENDS ${directory}
    OUTPUT ${output}
    COMMENT "Generating ${output}"
    VERBATIM)

  add_custom_target(v8-bytecodes-builtin-list-target DEPENDS ${output})

  add_library(v8-bytecodes-builtin-list INTERFACE)
  add_dependencies(v8-bytecodes-builtin-list v8-bytecodes-builtin-list-target)
  set_target_properties(v8-bytecodes-builtin-list
    PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${target-dir})

  #target_include_directories(v8-bytecodes-builtin-list INTERFACE ${target-dir})
  #target_sources(v8-bytecodes-builtin-list INTERFACE ${output})
endfunction()
