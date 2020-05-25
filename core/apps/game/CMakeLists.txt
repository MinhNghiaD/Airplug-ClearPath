# Compilcation rules for the game application
# 2020 - Lucca Rawlyk

set(game_SRCS
    main.cpp
    agent.cpp
    board.cpp
    world.cpp
    state.cpp
    agent_controller.cpp
)

add_executable(game ${game_SRCS})

set_target_properties(game PROPERTIES OUTPUT_NAME "game")

target_link_libraries(game
                      apg
                      ${QT_LIBRARIES}
                     )
