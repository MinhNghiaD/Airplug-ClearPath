cmake_minimum_required(VERSION 3.1.0)

project(Airplug)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Qt5 COMPONENTS Core    REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Gui     REQUIRED)
find_package(Qt5 COMPONENTS Test    REQUIRED)


get_target_property(Qt5Core_INCLUDE_DIRS    Qt5::Core    LOCATION)
get_target_property(Qt5Network_INCLUDE_DIRS Qt5::Network LOCATION)
get_target_property(Qt5Widgets_INCLUDE_DIRS Qt5::Widgets LOCATION)
get_target_property(Qt5Gui_INCLUDE_DIRS     Qt5::Gui     LOCATION)
get_target_property(Qt5Test_INCLUDE_DIRS    Qt5::Test    LOCATION)

set(QT_INCLUDES_DIRECTORIES ${Qt5Core_INCLUDE_DIRS}
                            ${Qt5Network_INCLUDE_DIRS}
                            ${Qt5Widgets_INCLUDE_DIRS}
                            ${Qt5Gui_INCLUDE_DIRS}
                            ${Qt5Test_INCLUDE_DIRS}
    )

set(QT_LIBRARIES Qt5::Core
                 Qt5::Network
                 Qt5::Widgets
                 Qt5::Gui
                 Qt5::Test
    )

message(STATUS "Qt includes directories: ${QT_INCLUDES_DIRECTORIES}")
message(STATUS "Qt libraries           : ${QT_LIBRARIES}")

set(libapg_includes_directories ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Frame/
                                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Transporters/
                                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Communication/
   )

include_directories(${libapg_includes_directories})

set(libapg_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Frame/header.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Frame/message.cpp

                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Transporters/message_transporter.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Transporters/std_transporter.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Transporters/udp_transporter.cpp

                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Communication/communication_manager.cpp
    )

add_library(apg SHARED ${libapg_SRCS})

set_target_properties(apg PROPERTIES LOCATION ${CMAKE_BINARY_DIR}/libapg.so)

target_link_libraries(apg ${QT_LIBRARIES})

# TODO: define order of linker
add_subdirectory(core)

add_subdirectory(tools)


#add_executable(helloworld
 #  mainwindow.ui
 #  mainwindow.cpp
 #  main.cpp
 #   resources.qrc
#)

#target_link_libraries(helloworld Qt5::Widgets)
