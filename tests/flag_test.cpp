#include <mcga/test.hpp>
#include <mcga/test_ext/matchers.hpp>

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
    a = parser->add_flag(FlagSpec("flag_a").set_short_name("a"));
    b = parser->add_flag(FlagSpec("flag_b").set_short_name("b"));
  });

  tearDown([&] {
    delete parser;
    parser = nullptr;
  });

  test("Default flag value is false", [&] {
    expect(a->get_value(), isFalse);
    expect(b->get_value(), isFalse);
  });

  test("Implicit flag value is true", [&] {
    parser->parse({"--flag_a"});
    expect(a->get_value(), isTrue);
    expect(b->get_value(), isFalse);

    parser->parse({"--a"});
    expect(a->get_value(), isTrue);
    expect(b->get_value(), isFalse);

    parser->parse({"-ab", "-a"});
    expect(a->get_value(), isTrue);
    expect(b->get_value(), isTrue);
  });

  test("Passing a flag value 'enabled' enables it", [&] {
    parser->parse({"--flag_a=enabled"});
    expect(a->get_value(), isTrue);
  });

  test("Passing a flag random values throws", [&] {
    expect(
        [&] {
          parser->parse({"--flag_a=whatever"});
        },
        throwsA<invalid_argument>());
  });

  test("Single dash flag does not associate with the following positional "
       "argument",
       [&] {
         auto positional = parser->parse({"-a", "enabled"});
         expect(positional, isEqualTo(vector<string>{"enabled"}));
         expect(a->get_value(), isTrue);
       });
}
