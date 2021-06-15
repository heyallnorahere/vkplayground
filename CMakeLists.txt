cmake_minimum_required(VERSION 3.20)
project(vkplayground)
# by default, the playground application will not be build; however, it will through the config script
option(BUILD_PLAYGROUND_APPLICATION "Decides whether or not the playground application should be built" OFF)
add_subdirectory("vendor")
add_subdirectory("lib")
if(BUILD_PLAYGROUND_APPLICATION)
    add_subdirectory("src")
endif()