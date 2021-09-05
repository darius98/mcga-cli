#pragma ide diagnostic ignored "google-runtime-int"
#pragma ide diagnostic ignored "readability-magic-numbers"

#include <mcga/test.hpp>
#include <mcga/test_ext/matchers.hpp>

#include "mcga/cli.hpp"

using mcga::cli::ListArgumentSpec;
using mcga::cli::NumericArgument;
using mcga::cli::Parser;
using mcga::matchers::isEqualTo;
using mcga::matchers::throwsA;
using mcga::test::expect;
using mcga::test::setUp;
using mcga::test::tearDown;
using mcga::test::test;
using std::invalid_argument;

TEST_CASE(McgaCliListArgument, "List argument") {
  Parser* parser = nullptr;

  setUp([&] {
    parser = new Parser("");
  });

  tearDown([&] {
    delete parser;
    parser = nullptr;
  });

  test("Empty default value", [&] {
    auto arg = parser->add_list_argument(
        ListArgumentSpec("name").set_default_value({}));

    parser->parse({});
    expect(arg->get_value(), isEqualTo(std::vector<std::string>{}));
  });

  test("Non-empty default value", [&] {
    auto arg = parser->add_list_argument(
        ListArgumentSpec("name").set_default_value({"a", "b", "c"}));

    parser->parse({});
    expect(arg->get_value(),
           isEqualTo(std::vector<std::string>{"a", "b", "c"}));
  });

  test("Applying single value", [&] {
    auto arg = parser->add_list_argument(ListArgumentSpec("name"));

    parser->parse({"--name=c"});
    expect(arg->get_value(), isEqualTo(std::vector<std::string>{"c"}));
  });

  test("Applying two values", [&] {
    auto arg = parser->add_list_argument(ListArgumentSpec("name"));

    parser->parse({"--name=c", "--name=d"});
    expect(arg->get_value(), isEqualTo(std::vector<std::string>{"c", "d"}));
  });

  test("Applying implicit value once", [&] {
    auto arg = parser->add_list_argument(
        ListArgumentSpec("name").set_implicit_value({"a", "b"}));

    parser->parse({"--name"});
    expect(arg->get_value(), isEqualTo(std::vector<std::string>{"a", "b"}));
  });

  test("Applying implicit value twice", [&] {
    auto arg = parser->add_list_argument(
        ListArgumentSpec("name").set_implicit_value({"a", "b"}));

    parser->parse({"--name", "--name"});
    expect(arg->get_value(), isEqualTo(std::vector<std::string>{"a", "b"}));
  });

  test("Applying implicit value before another value", [&] {
    auto arg = parser->add_list_argument(
        ListArgumentSpec("name").set_implicit_value({"a", "b"}));

    parser->parse({"--name", "--name=c"});
    expect(arg->get_value(),
           isEqualTo(std::vector<std::string>{"a", "b", "c"}));
  });

  test("Applying implicit value after another value", [&] {
    auto arg = parser->add_list_argument(
        ListArgumentSpec("name").set_implicit_value({"a", "b"}));

    parser->parse({"--name=c", "--name"});
    expect(arg->get_value(),
           isEqualTo(std::vector<std::string>{"c", "a", "b"}));
  });

  test("Applying implicit value and multiple other values", [&] {
    auto arg = parser->add_list_argument(
        ListArgumentSpec("name").set_implicit_value({"a", "b"}));

    parser->parse({"--name=c", "--name", "--name=d", "--name=q"});
    expect(arg->get_value(),
           isEqualTo(std::vector<std::string>{"c", "a", "b", "d", "q"}));
  });

  test("NumericArgument: Applying implicit value and multiple other values",
       [&] {
         auto arg = parser->add_list_argument(
             ListArgumentSpec<NumericArgument<int>>("name").set_implicit_value(
                 {"1", "2"}));

         parser->parse({"--name=3", "--name", "--name=4", "--name=5"});
         expect(arg->get_value(), isEqualTo(std::vector<int>{3, 1, 2, 4, 5}));
       });
}
