# Compilcation rules for Airplug project
# 2020 - Nghia Duong, <minhnghiaduong997 at gmail dot com>

cmake_minimum_required(VERSION 3.1.0)

project(Airplug)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Qt5 COMPONENTS Core    REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Gui     REQUIRED)
find_package(Qt5 COMPONENTS Test    REQUIRED)

set(QT_LIBRARIES Qt5::Core
                 Qt5::Network
                 Qt5::Widgets
                 Qt5::Gui
                 Qt5::Test
    )

message(STATUS "Qt libraries           : ${QT_LIBRARIES}")



set(libapg_includes_directories ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Frame/
                                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Transporters/
                                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Communication/
                                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/OptionParser/
                                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Controller/
                                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Clock/
                                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/Snapshot/
                                ${CMAKE_CURRENT_SOURCE_DIR}/core/libapg_qt/MutualExclusion/
   )




include_directories(${libapg_includes_directories})

add_subdirectory(core)

add_subdirectory(tools)
