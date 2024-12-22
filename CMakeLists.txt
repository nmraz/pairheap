cmake_minimum_required(VERSION 3.28)
project(pairheap C)

set(CMAKE_C_STANDARD 23)

add_compile_options(-Wall -Wextra)

set(FUZZ_OPTS -g -fsanitize=fuzzer,address)

add_executable(fuzztest src/pairheap.c src/fuzz.c)
target_compile_options(fuzztest PRIVATE ${FUZZ_OPTS})
target_link_options(fuzztest PRIVATE ${FUZZ_OPTS})
