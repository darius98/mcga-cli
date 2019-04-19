#include <mcga/matchers.hpp>
#include <mcga/test.hpp>

#include "mcga/cli.hpp"

using mcga::cli::Flag;
using mcga::cli::FlagSpec;
using mcga::cli::Parser;
using mcga::matchers::isEqualTo;
using mcga::matchers::isFalse;
using mcga::matchers::isTrue;
using mcga::matchers::throwsA;
using mcga::test::expect;
using mcga::test::setUp;
using mcga::test::tearDown;
using mcga::test::test;
using std::invalid_argument;
using std::string;
using std::vector;


TEST_CASE(McgaCliFlag, "Flag") {
    Parser* parser = nullptr;
    Flag a;
    Flag b;

    setUp([&] {
        parser = new Parser("");
        a = parser->addFlag(FlagSpec("flag_a").setShortName("a"));
        b = parser->addFlag(FlagSpec("flag_b").setShortName("b"));
    });

    tearDown([&] {
        delete parser;
        parser = nullptr;
    });

    test("Default flag value is false", [&] {
        expect(a->getValue(), isFalse);
        expect(b->getValue(), isFalse);
    });

    test("Implicit flag value is true", [&] {
        parser->parse({"--flag_a"});
        expect(a->getValue(), isTrue);
        expect(b->getValue(), isFalse);

        parser->parse({"--a"});
        expect(a->getValue(), isTrue);
        expect(b->getValue(), isFalse);

        parser->parse({"-ab", "-a"});
        expect(a->getValue(), isTrue);
        expect(b->getValue(), isTrue);
    });

    test("Passing a flag value 'enabled' enables it", [&] {
        parser->parse({"--flag_a=enabled"});
        expect(a->getValue(), isTrue);
    });

    test("Passing a flag random values throws", [&] {
        expect([&] {
            parser->parse({"--flag_a=whatever"});
        }, throwsA<invalid_argument>());
    });

    test("Single dash flag does not associate with the following "
         "positional argument", [&] {
        auto positional = parser->parse({"-a", "enabled"});
        expect(positional, isEqualTo(vector<string>{"enabled"}));
        expect(a->getValue(), isTrue);
    });
}
