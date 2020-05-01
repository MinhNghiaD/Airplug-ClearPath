add_subdirectory(BAS)
add_subdirectory(NET)

# copy network construction script to binary directory

add_custom_target(constructlink ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/network_scripts ${CMAKE_CURRENT_BINARY_DIR}/network_scripts)
