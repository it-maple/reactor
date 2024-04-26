cmake_minimum_required(VERSION 3.20)

project(reactor)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_11)
target_compile_options(compiler_flags INTERFACE -g)
target_compile_options(compiler_flags INTERFACE -D_GLIBCXX_DEBUG)
target_compile_options(compiler_flags INTERFACE -Wall)
target_compile_options(compiler_flags INTERFACE -Werror)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)