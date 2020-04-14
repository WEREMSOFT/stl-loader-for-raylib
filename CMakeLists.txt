cmake_minimum_required(VERSION 3.15)
project(raylib-flecs-spine C)

file(COPY assets DESTINATION ./)
set(CMAKE_C_STANDARD 99)

set(OPTIMIZATION_FLAGS -O0)

link_directories("${PROJECT_SOURCE_DIR}/lib/static")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPTIMIZATION_FLAGS} -lflecs_static -lraylib -framework OpenGL -framework GLUT -framework Cocoa -framework CoreVideo -framework IOKit -v")

set(ENV{PLATFORM_DESKTOP} true)

FILE(GLOB sources src/*)

add_executable(${PROJECT_NAME} ${sources})
target_include_directories(${PROJECT_NAME} PRIVATE lib/include)

