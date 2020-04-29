add_subdirectory(BAS)
add_subdirectory(NET)

# copy network construction script to binary directory

add_custom_target(constructlink ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/construct.sh ${CMAKE_CURRENT_BINARY_DIR}/construct.sh)

add_custom_target(cleanlink ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/clean_processes.sh ${CMAKE_CURRENT_BINARY_DIR}/clean_processes.sh)
