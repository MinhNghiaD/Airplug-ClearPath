# Compilcation rules for unit tests
# 2020 - Nghia Duong, <minhnghiaduong997 at gmail dot com>

MACRO(LIBAPG_TOOLS_BUILD)

    set(_filename ${ARGV0})
    string(REPLACE "." ";" _temp ${_filename})
    list(GET _temp 0 _target)

    set(${_target}_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/${_target}
        )

    add_executable(${_target} ${${_target}_SRCS})

    set_target_properties(${_target} PROPERTIES OUTPUT_NAME ${_target})

    target_link_libraries(${_target}
                          apg
                          extras
                          ${QT_LIBRARIES}
                         )
ENDMACRO()

# add unit tests hear:
LIBAPG_TOOLS_BUILD(test_message.cpp)
LIBAPG_TOOLS_BUILD(test_clock.cpp)
LIBAPG_TOOLS_BUILD(test_kdtree.cpp)
