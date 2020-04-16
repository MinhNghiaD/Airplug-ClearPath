set(libapg_includes_directories ${CMAKE_CURRENT_SOURCE_DIR}/Frame/
                                ${CMAKE_CURRENT_SOURCE_DIR}/Transporters/
                                ${CMAKE_CURRENT_SOURCE_DIR}/Communication/
   )

include_directories(${libapg_includes_directories})

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Frame/)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Transporters/)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Communication/)

set(libapg_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/Frame/header.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Frame/message.cpp

                ${CMAKE_CURRENT_SOURCE_DIR}/Transporters/message_transporter.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Transporters/std_transporter.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/Transporters/udp_transporter.cpp

                ${CMAKE_CURRENT_SOURCE_DIR}/Communication/communication_manager.cpp
    )

add_library(apg STATIC ${libapg_SRCS})

#set_target_properties(apg PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/libapg.so )

target_link_libraries(apg ${QT_LIBRARIES})
