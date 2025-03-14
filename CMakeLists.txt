cmake_minimum_required(VERSION 3.28)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project(lab)

set(CMAKE_C_STANDARD 11)

add_executable(main main.c)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

