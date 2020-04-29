add_subdirectory(BAS)
add_subdirectory(NET)

# copy network construction script to binary directory
file(COPY construct.sh
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
