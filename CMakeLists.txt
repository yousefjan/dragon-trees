cmake_minimum_required(VERSION 3.20)

project(dragon_trees LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 QUIET)

find_path(GLEW_INCLUDE_DIR GL/glew.h
    PATHS
        /opt/homebrew/include
        /usr/local/include
)

find_library(GLEW_LIBRARY
    NAMES GLEW glew
    PATHS
        /opt/homebrew/lib
        /usr/local/lib
)

if(NOT GLEW_INCLUDE_DIR OR NOT GLEW_LIBRARY)
    message(FATAL_ERROR "GLEW not found. Install with: brew install glew")
endif()

add_executable(dragon_trees
    src/tree.cpp
    src/gl_present.cpp
)

target_include_directories(dragon_trees PRIVATE
    ${GLEW_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(TARGET glfw)
    target_link_libraries(dragon_trees PRIVATE OpenGL::GL glfw ${GLEW_LIBRARY})
else()
    find_library(GLFW_LIBRARY
        NAMES glfw glfw3
        PATHS
            /opt/homebrew/lib
            /usr/local/lib
    )
    if(NOT GLFW_LIBRARY)
        message(FATAL_ERROR "GLFW not found. Install with: brew install glfw")
    endif()
    target_link_libraries(dragon_trees PRIVATE OpenGL::GL ${GLFW_LIBRARY} ${GLEW_LIBRARY})
endif()

if(APPLE)
    target_link_libraries(dragon_trees PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()
