cmake_minimum_required(VERSION 3.5)
project(FactoryGame)
set_property(GLOBAL PROPERTY C_STANDARD 21)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${Vulkan_INCLUDE_DIRS})
add_executable(FactoryGame main.c)
target_link_libraries(FactoryGame ${Vulkan_LIBRARY} glfw)
