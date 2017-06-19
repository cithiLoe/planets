cmake_minimum_required (VERSION 2.6)
project (planets)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic")

include_directories(include)

find_package(OpenGL REQUIRED)

add_executable(planets
    src/main.cpp
    src/shader.cpp
    src/camera.cpp
    src/planet.cpp
    src/texture.cpp
    src/mesh.cpp
    src/model.cpp
    )

set(ALL_LIBS
    ${OPENGL_LIBRARY}
    glfw
	GLEW
    assimp
)

target_link_libraries(planets
    ${ALL_LIBS}
)
