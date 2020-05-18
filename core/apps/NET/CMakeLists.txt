# Compilcation rules for NET application
# 2020 - Nghia Duong, <minhnghiaduong997 at gmail dot com>

set(net_SRCS
    net_controller.cpp
    mainwindow.ui
    mainwindow.cpp
    main.cpp
)

add_executable(net ${net_SRCS})

set_target_properties(net PROPERTIES OUTPUT_NAME "net")

target_link_libraries(net
                      apg
                      ${QT_LIBRARIES}
                     )
