function(add_example folder_name target_name source_file)
  add_executable(${target_name})
  set_target_properties(${target_name} PROPERTIES FOLDER ${folder_name})

  target_sources(${target_name}
  PRIVATE
    ${source_file}
  )

  target_link_libraries(${target_name} PRIVATE pcp::pcp)

  include(GNUInstallDirs)

  install(
    TARGETS ${target_name}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
endfunction()

add_example("examples" pcp-normals-estimation normals_estimation.cpp)
add_example("examples" pcp-ply ply.cpp)