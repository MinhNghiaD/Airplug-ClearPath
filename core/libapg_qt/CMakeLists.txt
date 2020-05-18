# Compilcation rules for libapg
# 2020 - Nghia Duong, <minhnghiaduong997 at gmail dot com>

set(libapg_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Frame/header.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Frame/message.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Frame/aclmessage.cpp

                ${CMAKE_CURRENT_SOURCE_DIR}/Transporters/message_transporter.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Transporters/std_transporter.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Transporters/udp_transporter.cpp

                ${CMAKE_CURRENT_SOURCE_DIR}/Communication/communication_manager.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Communication/router.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Communication/watchdog.cpp

                ${CMAKE_CURRENT_SOURCE_DIR}/OptionParser/option_parser.cpp

                ${CMAKE_CURRENT_SOURCE_DIR}/Controller/application_controller.cpp

                ${CMAKE_CURRENT_SOURCE_DIR}/Clock/vector_clock.cpp

                ${CMAKE_CURRENT_SOURCE_DIR}/Snapshot/laiyang_snapshot.cpp

                ${CMAKE_CURRENT_SOURCE_DIR}/MutualExclusion/ricart_lock.cpp
    )

add_library(apg STATIC ${libapg_SRCS})

target_link_libraries(apg ${QT_LIBRARIES})
