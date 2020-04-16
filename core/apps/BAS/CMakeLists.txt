set(bas_SRCS
    mainwindow.ui
    mainwindow.cpp
    main.cpp
)

add_executable(bas ${bas_SRCS})

set_target_properties(bas PROPERTIES OUTPUT_NAME "bas")

target_link_libraries(bas
                      "${PROJECT_BINARY_DIR}/libapg.so"
                      ${QT_LIBRARIES}
                     )

target_include_directories(bas PUBLIC
                           "${PROJECT_BINARY_DIR}"
                          )
