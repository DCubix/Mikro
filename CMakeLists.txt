cmake_minimum_required(VERSION 3.10)
project(mik)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

add_definitions(-DSDL_MAIN_HANDLED)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.c")

find_package(SDL2 CONFIG REQUIRED)
find_package(Lua REQUIRED Lua)

add_executable(${PROJECT_NAME} ${SRC})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 Lua)

