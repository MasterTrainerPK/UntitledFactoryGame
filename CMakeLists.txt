cmake_minimum_required(VERSION 3.0)
project(FactoryGame)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${VULKAN_INCLUDE_DIRS})
add_executable(FactoryGame main.c)
target_link_libraries(FactoryGame ${VULKAN_Libraries} glfw)
