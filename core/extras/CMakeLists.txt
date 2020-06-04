# Compilcation rules for extra librarys
# 2020 - Nghia Duong, <minhnghiaduong997 at gmail dot com>

set(extras_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/clearpath/kd_node.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/clearpath/kd_tree.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/clearpath/rvo.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/clearpath/collision_avoidance_manager.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/clearpath/environment_manager.cpp
    )

add_library(extras STATIC ${extras_SRCS})

target_link_libraries(extras ${QT_LIBRARIES})
