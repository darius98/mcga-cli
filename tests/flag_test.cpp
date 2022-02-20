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

TEST_CASE("Flag") {
  std::unique_ptr<Parser> parser;
  Flag a;
  Flag b;

  setUp([&] {
    parser = std::make_unique<Parser>("");
    a = parser->add_flag(FlagSpec("flag_a").set_short_name("a"));
    b = parser->add_flag(FlagSpec("flag_b").set_short_name("b"));
  });

  tearDown([&] {
    parser.reset();
  });

  test("Default flag value is false", [&] {
    parser->parse({});
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
        throwsA<std::invalid_argument>);
  });

  test("Single dash flag does not associate with the following positional "
       "argument",
       [&] {
         auto positional = parser->parse({"-a", "enabled"});
         expect(positional, isEqualTo(std::vector<std::string>{"enabled"}));
         expect(a->get_value(), isTrue);
       });
}
