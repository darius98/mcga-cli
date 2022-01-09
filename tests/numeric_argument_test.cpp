#include <mcga/test.hpp>
#include <mcga/test_ext/matchers.hpp>

#include "mcga/cli.hpp"

using mcga::cli::NumericArgumentSpec;
using mcga::cli::Parser;
using mcga::matchers::isEqualTo;
using mcga::matchers::throwsA;
using mcga::test::expect;
using mcga::test::setUp;
using mcga::test::tearDown;
using mcga::test::test;
using mcga::test::TestCase;

static auto t = TestCase{"Numeric argument"} + [] {
  std::unique_ptr<Parser> parser;

  setUp([&] {
    parser = std::make_unique<Parser>("");
  });

  tearDown([&] {
    parser.reset();
  });

  test("Passing a NumericArgument<int> an integer value works", [&] {
    auto arg = parser->add_numeric_argument<int>(NumericArgumentSpec("name"));

    parser->parse({"--name=17"});
    expect(arg->get_value(), isEqualTo(17));

    parser->parse({"--name=-7"});
    expect(arg->get_value(), isEqualTo(-7));

    parser->parse({"--name=1337"});
    expect(arg->get_value(), isEqualTo(1337));
  });

  test("Passing a NumericArgument<int> a non-integer value throws", [&] {
    parser->add_numeric_argument<int>(NumericArgumentSpec("name"));
    expect(
        [&] {
          parser->parse({"--name=invalid"});
        },
        throwsA<std::invalid_argument>());
  });

  test("Passing a NumericArgument<int> a negative value works", [&] {
    auto arg = parser->add_numeric_argument<int>(NumericArgumentSpec("name"));
    parser->parse({"--name=-1337"});
    expect(arg->get_value(), isEqualTo(-1337));
  });

  test("Passing a NumericArgument<long long> a 64-bit integer value works",
       [&] {
         auto arg = parser->add_numeric_argument<long long>(
             NumericArgumentSpec("name"));
         parser->parse({"--name=12345678912345"});
         expect(arg->get_value(), isEqualTo(12345678912345LL));
       });
};
