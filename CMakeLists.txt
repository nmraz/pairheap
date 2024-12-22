cmake_minimum_required(VERSION 3.28)
project(pairheap C)

set(CMAKE_C_STANDARD 23)

add_compile_options(-Wall -Wextra)
add_executable(pairheaptest src/pairheap.c src/main.c)
