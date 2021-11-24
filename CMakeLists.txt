cmake_minimum_required(VERSION 3.18)
project(MCGA_cli)
set(CMAKE_CXX_STANDARD 20)

option(MCGA_cli_tests "Build MCGA CLI tests" OFF)

add_library(mcga_cli STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/argument.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/command_line_option.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/exceptions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/flag.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/generator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/numeric_argument.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp)
target_include_directories(mcga_cli PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (MCGA_cli_tests)
    add_executable(mcga_cli_test
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/choice_argument_test.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/flag_test.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/help_test.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/list_argument_test.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/numeric_argument_test.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/parser_test.cpp
            )
    target_link_libraries(mcga_cli_test mcga_test mcga_cli)
endif ()

install(DIRECTORY include DESTINATION .)
install(TARGETS mcga_cli DESTINATION lib)
