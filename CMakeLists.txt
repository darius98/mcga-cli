cmake_minimum_required(VERSION 3.12.1)

option(MCGA_cli_tests "Build MCGA CLI tests" OFF)
option(MCGA_cli_examples "Build MCGA CLI examples" OFF)

add_library(mcga_cli INTERFACE)
target_include_directories(mcga_cli INTERFACE include)

install(DIRECTORY include DESTINATION .)

if (MCGA_cli_tests)
    # TODO(darius98): Remove this once kktest is part of MCGA.
    link_directories(/usr/local/lib)

    add_executable(mcga_cli_test
        tests/choice_argument_test.cpp
        tests/flag_test.cpp
        tests/help_test.cpp
        tests/numeric_argument_test.cpp
        tests/parser_test.cpp
    )
    # TODO(darius98): Remove this once kktest is part of MCGA.
    target_include_directories(mcga_cli_test PRIVATE /usr/local/include)
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