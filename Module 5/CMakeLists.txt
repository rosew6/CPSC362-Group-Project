cmake_minimum_required(VERSION 3.20)

project(CFG_ParserRecognizer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(CFG_ParserRecognizer
    ConsoleApplication1.cpp
)

if (MSVC)
    target_compile_options(CFG_ParserRecognizer PRIVATE
        /W4
    )
else()
    target_compile_options(CFG_ParserRecognizer PRIVATE
        -Wall
        -Wextra
        -pedantic
    )
endif()
