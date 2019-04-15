cmake_minimum_required(VERSION 3.12.1)
project(MCGA_cli)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

option(MCGA_cli_tests "Build MCGA CLI tests" OFF)
option(MCGA_cli_examples "Build MCGA CLI examples" OFF)

add_library(mcga_cli INTERFACE)
target_include_directories(mcga_cli INTERFACE include)

install(DIRECTORY include DESTINATION .)

if (MCGA_cli_tests)
    add_executable(mcga_cli_test
        tests/choice_argument_test.cpp
        tests/flag_test.cpp
        tests/help_test.cpp
        tests/numeric_argument_test.cpp
        tests/parser_test.cpp
    )
    target_link_libraries(mcga_cli_test kktest mcga_cli)
endif ()

if (MCGA_cli_examples)
    function (add_example example_name)
        add_executable(mcga_cli_${example_name}_example
            examples/${example_name}.cpp
        )
        target_link_libraries(mcga_cli_${example_name}_example mcga_cli)
    endfunction ()

    add_example(basic)
endif ()