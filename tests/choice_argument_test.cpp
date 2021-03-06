#pragma ide diagnostic ignored "readability-magic-numbers"

#include <mcga/test.hpp>
#include <mcga/test_ext/matchers.hpp>

#include "mcga/cli.hpp"

using mcga::cli::ChoiceArgumentSpec;
using mcga::cli::Parser;
using mcga::matchers::isEqualTo;
using mcga::matchers::throwsA;
using mcga::test::expect;
using mcga::test::setUp;
using mcga::test::tearDown;
using mcga::test::test;
using std::invalid_argument;
using std::map;
using std::string;

TEST_CASE(McgaCliChoiceArgument, "Choice argument") {
  Parser* parser = nullptr;

  setUp([&] {
    parser = new Parser("");
  });

  tearDown([&] {
    delete parser;
    parser = nullptr;
  });

  test("String choice argument, setting valid value", [&] {
    auto arg = parser->add_choice_argument(
        ChoiceArgumentSpec<string>("name").add_option("value", "value"));
    parser->parse({"--name=value"});
    expect(arg->get_value(), isEqualTo("value"));
  });

  test("String choice argument, setting invalid value throws", [&] {
    parser->add_choice_argument(
        ChoiceArgumentSpec<string>("name").add_option("value", "value"));
    expect(
        [&] {
          parser->parse({"--name=other"});
        },
        throwsA<invalid_argument>());
  });

  test("Argument with multiple choices, mapping to the same value", [&] {
    auto arg = parser->add_choice_argument(ChoiceArgumentSpec<string>("name")
                                               .add_option("key1", "value")
                                               .add_option("key2", "value"));

    parser->parse({"--name=key1"});
    expect(arg->get_value(), isEqualTo("value"));

    parser->parse({"--name=key2"});
    expect(arg->get_value(), isEqualTo("value"));
  });

  test("Argument that maps to integers", [&] {
    auto arg = parser->add_choice_argument(
        ChoiceArgumentSpec<int>("name").add_option("1", 1).add_option("ONE",
                                                                      1));
    parser->parse({"--name=1"});
    expect(arg->get_value(), isEqualTo(1));

    parser->parse({"--name=ONE"});
    expect(arg->get_value(), isEqualTo(1));
  });

  test("ArgumentSpec options manipulation", [&] {
    ChoiceArgumentSpec<int> spec("name");

    expect(spec.options, isEqualTo(map<string, int>{}));

    spec.add_option("key", 1).add_option("key2", 2);
    expect(spec.options, isEqualTo(map<string, int>{{"key", 1}, {"key2", 2}}));

    spec.add_options({{"key", 3}, {"key3", 4}});
    expect(spec.options,
           isEqualTo(map<string, int>{{"key", 3}, {"key2", 2}, {"key3", 4}}));

    spec.set_options({{"k", 12}, {"l", 14}});
    expect(spec.options, isEqualTo(map<string, int>{{"k", 12}, {"l", 14}}));
  });
}