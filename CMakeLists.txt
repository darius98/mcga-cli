cmake_minimum_required(VERSION 3.12.1)
project(MCGA_cli)
set(CMAKE_CXX_STANDARD 17)

option(MCGA_cli_tests "Build MCGA CLI tests" OFF)
option(MCGA_cli_examples "Build MCGA CLI examples" OFF)

add_library(mcga_cli STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/argument.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/command_line_spec.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/flag.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/generator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/list_argument.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/numeric_argument.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp)
target_include_directories(mcga_cli PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(DIRECTORY include DESTINATION .)

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

if (MCGA_cli_examples)
    function(add_example example_name)
        add_executable(mcga_cli_${example_name}_example
                ${CMAKE_CURRENT_SOURCE_DIR}/examples/${example_name}.cpp
                )
        target_link_libraries(mcga_cli_${example_name}_example mcga_cli)
    endfunction()

    add_example(basic)
endif ()