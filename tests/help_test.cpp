#include <vector>

#include <mcga/test.hpp>
#include <mcga/test_ext/matchers.hpp>

#include "mcga/cli.hpp"

using mcga::cli::ArgumentSpec;
using mcga::cli::FlagSpec;
using mcga::cli::NumericArgumentSpec;
using mcga::cli::Parser;
using mcga::matchers::isEqualTo;

TEST_CASE("Help") {
  std::unique_ptr<Parser> parser;

  setUp([&] {
    parser = std::make_unique<Parser>("Test help prefix.");
    parser->add_help_flag();
  });

  tearDown([&] {
    parser.reset();
  });

  test("Only with the help flag", [&] {
    expect(parser->render_help(),
           isEqualTo("Test help prefix.\n"
                     "\n"
                     "\t--help,-h  Display this help menu.\n"));
  });

  test("After adding another flag without help group", [&] {
    parser->add_flag(FlagSpec("version")
                         .set_description("Display program version")
                         .set_short_name("v"));
    expect(parser->render_help(),
           isEqualTo("Test help prefix.\n"
                     "\n"
                     "\t--help,-h  Display this help menu.\n"
                     "\t--version,-v  Display program version\n"));
  });

  test("After adding an argument with default and implicit values", [&] {
    parser->add_argument(ArgumentSpec("config")
                             .set_description("File to take config from")
                             .set_default_value("/path/to/default-config.txt")
                             .set_implicit_value("./config.txt"));
    expect(parser->render_help(),
           isEqualTo("Test help prefix.\n"
                     "\n"
                     "\t--help,-h  Display this help menu.\n"
                     "\t--config  File to take config from\n"
                     "\t\tDefault: '/path/to/default-config.txt', "
                     "Implicit: './config.txt'\n"));
  });

  test("After adding argument within a group", [&] {
    parser->add_argument(ArgumentSpec("config")
                             .set_description("File to take config from")
                             .set_help_group("Config")
                             .set_default_value("/path/to/default-config.txt")
                             .set_implicit_value("./config.txt"));
    expect(parser->render_help(),
           isEqualTo("Test help prefix.\n"
                     "\n"
                     "\t--help,-h  Display this help menu.\n"
                     "\n"
                     "Config\n"
                     "\t--config  File to take config from\n"
                     "\t\tDefault: '/path/to/default-config.txt', "
                     "Implicit: './config.txt'\n"));
  });

  test("Arguments in a group are in the same order as they were added.", [&] {
    parser->add_argument(ArgumentSpec("config")
                             .set_description("File to take config from")
                             .set_help_group("Config")
                             .set_default_value("/path/to/default-config.txt")
                             .set_implicit_value("./config.txt"));
    parser->add_argument(ArgumentSpec("json-config")
                             .set_description("File to take JSON config from")
                             .set_help_group("Config")
                             .set_default_value("/path/to/default-config.json")
                             .set_implicit_value("./config.json"));

    expect(parser->render_help(),
           isEqualTo("Test help prefix.\n"
                     "\n"
                     "\t--help,-h  Display this help menu.\n"
                     "\n"
                     "Config\n"
                     "\t--config  File to take config from\n"
                     "\t\tDefault: '/path/to/default-config.txt', "
                     "Implicit: './config.txt'\n"
                     "\t--json-config  File to take JSON config from\n"
                     "\t\tDefault: '/path/to/default-config.json', "
                     "Implicit: './config.json'\n"));
  });

  test("Groups are in the same order as their first arguments added", [&] {
    parser->add_argument(ArgumentSpec("config")
                             .set_description("File to take config from")
                             .set_help_group("Config")
                             .set_default_value("/path/to/default-config.txt")
                             .set_implicit_value("./config.txt"));

    parser->add_flag(FlagSpec("version")
                         .set_description("Display program version")
                         .set_short_name("v"));

    parser->add_argument(ArgumentSpec("interpreter")
                             .set_description("Interpreter to use")
                             .set_help_group("Runtime")
                             .set_short_name("I")
                             .set_default_value("python3")
                             .set_implicit_value("python3"));

    parser->add_numeric_argument<int>(
        NumericArgumentSpec("vm-heap")
            .set_description("Interpreter VM max heap size")
            .set_help_group("Runtime")
            .set_default_value("1000")
            .set_implicit_value("1000"));

    parser->add_argument(ArgumentSpec("json-config")
                             .set_description("File to take JSON config from")
                             .set_help_group("Config")
                             .set_default_value("/path/to/default-config.json")
                             .set_implicit_value("./config.json"));

    expect(parser->render_help(),
           isEqualTo("Test help prefix.\n"
                     "\n"
                     "\t--help,-h  Display this help menu.\n"
                     "\t--version,-v  Display program version\n"
                     "\n"
                     "Config\n"
                     "\t--config  File to take config from\n"
                     "\t\tDefault: '/path/to/default-config.txt', "
                     "Implicit: './config.txt'\n"
                     "\t--json-config  File to take JSON config from\n"
                     "\t\tDefault: '/path/to/default-config.json', "
                     "Implicit: './config.json'\n"
                     "\n"
                     "Runtime\n"
                     "\t--interpreter,-I  Interpreter to use\n"
                     "\t\tDefault: 'python3', Implicit: 'python3'\n"
                     "\t--vm-heap  Interpreter VM max heap size\n"
                     "\t\tDefault: '1000', Implicit: '1000'\n"));
  });
}