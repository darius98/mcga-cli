#pragma ide diagnostic ignored "readability-magic-numbers"

#include <mcga/test.hpp>
#include <mcga/test_ext/matchers.hpp>

#include "mcga/cli.hpp"

using mcga::cli::ChoiceArgumentSpec;
using mcga::cli::Parser;
using mcga::test::expect;
using mcga::test::setUp;
using mcga::test::tearDown;
using mcga::test::test;
using mcga::test::matchers::isEqualTo;
using mcga::test::matchers::throwsA;
using std::invalid_argument;
using std::map;
using std::string;

TEST_CASE(McgaCliChoiceArgument, "Choice argument") {
    Parser* parser = nullptr;

    setUp([&] { parser = new Parser(""); });

    tearDown([&] {
        delete parser;
        parser = nullptr;
    });

    test("String choice argument, setting valid value", [&] {
        auto arg = parser->addChoiceArgument(
          ChoiceArgumentSpec<string>("name").addOption("value", "value"));
        parser->parse({"--name=value"});
        expect(arg->getValue(), isEqualTo("value"));
    });

    test("String choice argument, setting invalid value throws", [&] {
        parser->addChoiceArgument(
          ChoiceArgumentSpec<string>("name").addOption("value", "value"));
        expect([&] { parser->parse({"--name=other"}); },
               throwsA<invalid_argument>());
    });

    test("Argument with multiple choices, mapping to the same value", [&] {
        auto arg = parser->addChoiceArgument(ChoiceArgumentSpec<string>("name")
                                               .addOption("key1", "value")
                                               .addOption("key2", "value"));

        parser->parse({"--name=key1"});
        expect(arg->getValue(), isEqualTo("value"));

        parser->parse({"--name=key2"});
        expect(arg->getValue(), isEqualTo("value"));
    });

    test("Argument that maps to integers", [&] {
        auto arg = parser->addChoiceArgument(
          ChoiceArgumentSpec<int>("name").addOption("1", 1).addOption("ONE",
                                                                      1));
        parser->parse({"--name=1"});
        expect(arg->getValue(), isEqualTo(1));

        parser->parse({"--name=ONE"});
        expect(arg->getValue(), isEqualTo(1));
    });

    test("ArgumentSpec options manipulation", [&] {
        ChoiceArgumentSpec<int> spec("name");

        expect(spec.options, isEqualTo(map<string, int>{}));

        spec.addOption("key", 1).addOption("key2", 2);
        expect(spec.options,
               isEqualTo(map<string, int>{{"key", 1}, {"key2", 2}}));

        spec.addOptions({{"key", 3}, {"key3", 4}});
        expect(
          spec.options,
          isEqualTo(map<string, int>{{"key", 3}, {"key2", 2}, {"key3", 4}}));

        spec.setOptions({{"k", 12}, {"l", 14}});
        expect(spec.options, isEqualTo(map<string, int>{{"k", 12}, {"l", 14}}));
    });
}