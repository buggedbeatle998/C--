cmake_minimum_required(VERSION 3.16)
project("g--" LANGUAGES C VERSION 0.1.0 DESCRIPTION "cmm compiler")

set(CMAKE_C_STANDARD 23)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
file(GLOB SOURCE_FILES
    "src/*.c"
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
