cmake_minimum_required(VERSION 3.15)
project("RCPU16" C)

set(C_STANDARD 89)
set(C_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    src/main.c
    src/cpu.c
)