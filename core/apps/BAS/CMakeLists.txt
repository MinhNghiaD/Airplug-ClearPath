# Compilcation rules for BAS application
# 2020 - Nghia Duong, <minhnghiaduong997 at gmail dot com>

set(bas_SRCS
    mainwindow.ui
    mainwindow.cpp
    main.cpp
)

add_executable(bas ${bas_SRCS})

set_target_properties(bas PROPERTIES OUTPUT_NAME "bas")

target_link_libraries(bas
                      apg
                      ${QT_LIBRARIES}
                     )
