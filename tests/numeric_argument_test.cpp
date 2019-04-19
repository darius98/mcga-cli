#pragma ide diagnostic ignored "google-runtime-int"
#pragma ide diagnostic ignored "readability-magic-numbers"

#include <mcga/matchers.hpp>
#include <mcga/test.hpp>

#include "mcga/cli.hpp"

using mcga::cli::NumericArgumentSpec;
using mcga::cli::Parser;
using mcga::matchers::isEqualTo;
using mcga::matchers::throwsA;
using mcga::test::expect;
using mcga::test::setUp;
using mcga::test::tearDown;
using mcga::test::test;
using std::invalid_argument;

TEST_CASE(McgaCliNumericArgument, "Numeric argument") {
    Parser* parser = nullptr;

    setUp([&] { parser = new Parser(""); });

    tearDown([&] {
        delete parser;
        parser = nullptr;
    });

    test("Passing a NumericArgument<int> an integer value works", [&] {
        auto arg = parser->addNumericArgument(NumericArgumentSpec<int>("name"));

        parser->parse({"--name=17"});
        expect(arg->getValue(), isEqualTo(17));

        parser->parse({"--name=-7"});
        expect(arg->getValue(), isEqualTo(-7));

        parser->parse({"--name=1337"});
        expect(arg->getValue(), isEqualTo(1337));
    });

    test("Passing a NumericArgument<int> a non-integer value throws", [&] {
        parser->addNumericArgument(NumericArgumentSpec<int>("name"));
        expect([&] { parser->parse({"--name=invalid"}); },
               throwsA<invalid_argument>());
    });

    test("Passing a NumericArgument<int> a negative value works", [&] {
        auto arg = parser->addNumericArgument(NumericArgumentSpec<int>("name"));
        parser->parse({"--name=-1337"});
        expect(arg->getValue(), isEqualTo(-1337));
    });

    test("Passing a NumericArgument<long long> a 64-bit integer value works",
         [&] {
             auto arg = parser->addNumericArgument(
               NumericArgumentSpec<long long>("name"));
             parser->parse({"--name=12345678912345"});
             expect(arg->getValue(), isEqualTo(12345678912345LL));
         });
}
