MACRO(LIBAPG_TOOLS_BUILD)

    set(_filename ${ARGV0})
    string(REPLACE "." ";" _temp ${_filename})
    list(GET _temp 0 _target)

    set(${_target}_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/${_target}
        )

    add_executable(${_target} ${${_target}_SRCS})

    set_target_properties(${_target} PROPERTIES OUTPUT_NAME ${_target})

    #find_library(libapg_location ${libapg})

    target_link_libraries(${_target}
                          "${PROJECT_BINARY_DIR}/libapg.so"
                          ${QT_LIBRARIES}
                         )

    target_include_directories(${_target} PUBLIC
                               "${PROJECT_BINARY_DIR}"
                              )
ENDMACRO()

LIBAPG_TOOLS_BUILD(test_message.cpp)
