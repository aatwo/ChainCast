cmake_minimum_required(VERSION 3.5)
project(ChainCast)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_IMAGE REQUIRED SDL2_image)
pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)
include_directories(${SDL2_INCLUDE_DIRS})

set(SOURCE_FILES
        global.cpp
        global.h
        logger.cpp
        logger.h
        main.cpp
        mainwindow.cpp
        mainwindow.h
        sdlapplication.cpp
        sdlapplication.h
        sdleventdelegate.cpp
        sdleventdelegate.h
        )

add_executable(ChainCast ${SOURCE_FILES})
target_link_libraries(ChainCast ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES})
