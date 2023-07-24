cmake_minimum_required(VERSION 3.0)
project(FactoryGame)

find_package(SDL2 REQUIRED)
incude_directories(${SDL2_INCLUDE_DIRS})
add_executable(FactoryGame Main.c)
target_link_libraries(FactoryGame ${SDL2_Libraries})
