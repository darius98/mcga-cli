#include <mcga/test.hpp>
#include <mcga/test_ext/matchers.hpp>

#include "mcga/cli.hpp"

using mcga::cli::Argument;
using mcga::cli::ArgumentSpec;
using mcga::cli::Parser;
using mcga::matchers::isEqualTo;
using mcga::matchers::isFalse;
using mcga::matchers::isTrue;
using mcga::matchers::throwsA;
using mcga::test::expect;
using mcga::test::group;
using mcga::test::setUp;
using mcga::test::tearDown;
using mcga::test::test;
using std::invalid_argument;
using std::runtime_error;
using std::string;
using std::vector;

TEST_CASE(McgaCliParser, "Parser") {
  Parser* parser = nullptr;

  setUp([&] {
    parser = new Parser("Help prefix.");
  });

  tearDown([&] {
    delete parser;
    parser = nullptr;
  });

  group("Single argument", [&] {
    Argument arg;

    setUp([&] {
      arg = parser->add_argument(ArgumentSpec("name")
                                     .set_short_name("n")
                                     .set_default_value("a")
                                     .set_implicit_value("b"));
    });

    test("no value provided leads to argument taking default value", [&] {
      parser->parse({});
      expect(arg->get_value(), isEqualTo("a"));
      expect(arg->appeared(), isFalse);
    });

    test("provided with single dash, short name", [&] {
      parser->parse({"-n"});
      expect(arg->get_value(), isEqualTo("b"));
      expect(arg->appeared(), isTrue);
    });

    test("provided with double dash, short name", [&] {
      parser->parse({"--n"});
      expect(arg->get_value(), isEqualTo("b"));
      expect(arg->appeared(), isTrue);
    });

    test("provided with double dash, long name", [&] {
      parser->parse({"--name"});
      expect(arg->get_value(), isEqualTo("b"));
      expect(arg->appeared(), isTrue);
    });

    test("value provided with single dash & space", [&] {
      parser->parse({"-n", "v"});
      expect(arg->get_value(), isEqualTo("v"));
      expect(arg->appeared(), isTrue);
    });

    test("value provided with single dash & equal sign", [&] {
      parser->parse({"-n=v"});
      expect(arg->get_value(), isEqualTo("v"));
      expect(arg->appeared(), isTrue);
    });

    test("value provided with double dash & space is positional, while "
         "the argument takes implicit value",
         [&] {
           auto positionalArgs = parser->parse({"--name", "v"});
           expect(arg->get_value(), isEqualTo("b"));
           expect(positionalArgs, isEqualTo(vector<string>{"v"}));
           expect(arg->appeared(), isTrue);
         });

    test("value provided with double dash & equal sign (short name)", [&] {
      parser->parse({"--n=v"});
      expect(arg->get_value(), isEqualTo("v"));
      expect(arg->appeared(), isTrue);
    });

    test("value provided with double dash & equal sign (long name)", [&] {
      parser->parse({"--name=v"});
      expect(arg->get_value(), isEqualTo("v"));
      expect(arg->appeared(), isTrue);
    });

    test("providing value for different argument name does not influence"
         " interesting argument",
         [&] {
           parser->parse({"-m", "v"});
           expect(arg->get_value(), isEqualTo("a"));
           expect(arg->appeared(), isFalse);
         });

    test("when providing multiple values for one argument, it takes the "
         "last one",
         [&] {
           parser->parse({"-n", "v1", "-n", "--name=v2"});
           expect(arg->get_value(), isEqualTo("v2"));
           expect(arg->appeared(), isTrue);
         });
  });

  group("Multiple arguments", [&] {
    Argument a;
    Argument b;
    Argument c;

    setUp([&] {
      a = parser->add_argument(ArgumentSpec("arg_a")
                                   .set_short_name("a")
                                   .set_default_value("default")
                                   .set_implicit_value("implicit"));
      b = parser->add_argument(ArgumentSpec("arg_b")
                                   .set_short_name("b")
                                   .set_default_value("default")
                                   .set_implicit_value("implicit"));
      c = parser->add_argument(ArgumentSpec("arg_c")
                                   .set_short_name("c")
                                   .set_default_value("default")
                                   .set_implicit_value("implicit"));
    });

    test("Providing values for multiple arguments via double dash", [&] {
      parser->parse({"--arg_a=value", "--b"});
      expect(a->get_value(), isEqualTo("value"));
      expect(b->get_value(), isEqualTo("implicit"));
      expect(c->get_value(), isEqualTo("default"));
    });

    test("Providing values for multiple arguments with multiple single "
         "dash arguments",
         [&] {
           parser->parse({"-a", "-b", "value"});
           expect(a->get_value(), isEqualTo("implicit"));
           expect(b->get_value(), isEqualTo("value"));
           expect(c->get_value(), isEqualTo("default"));
         });

    test("Providing implicit values for multiple arguments via a single"
         " dash argument",
         [&] {
           parser->parse({"-ab"});
           expect(a->get_value(), isEqualTo("implicit"));
           expect(b->get_value(), isEqualTo("implicit"));
           expect(c->get_value(), isEqualTo("default"));
           expect(a->appeared(), isTrue);
           expect(b->appeared(), isTrue);
           expect(c->appeared(), isFalse);
         });

    test("Providing values for multiple arguments via a single dash"
         "argument & space for non-implicit value of the last one",
         [&] {
           parser->parse({"-abc", "value"});
           expect(a->get_value(), isEqualTo("implicit"));
           expect(b->get_value(), isEqualTo("implicit"));
           expect(c->get_value(), isEqualTo("value"));
           expect(a->appeared(), isTrue);
           expect(b->appeared(), isTrue);
           expect(c->appeared(), isTrue);
         });

    test("Providing values for multiple arguments via a single dash"
         "argument & equal sign for non-implicit value of the last one",
         [&] {
           parser->parse({"-abc=value"});
           expect(a->get_value(), isEqualTo("implicit"));
           expect(b->get_value(), isEqualTo("implicit"));
           expect(c->get_value(), isEqualTo("value"));
           expect(a->appeared(), isTrue);
           expect(b->appeared(), isTrue);
           expect(c->appeared(), isTrue);
         });
  });

  group("Invalid argument names", [&] {
    test("Registering an argument with the same name as an existing one "
         "throws",
         [&] {
           parser->add_argument(ArgumentSpec("name"));
           expect(
               [&] {
                 parser->add_argument(ArgumentSpec("name"));
               },
               throwsA<runtime_error>());
         });

    test("Registering an argument with the same name as an existing one's "
         "short name throws",
         [&] {
           parser->add_argument(ArgumentSpec("name").set_short_name("n"));
           expect(
               [&] {
                 parser->add_argument(ArgumentSpec("n"));
               },
               throwsA<runtime_error>());
         });

    test("Registering an argument with the same short name as an existing "
         "one's name throws",
         [&] {
           parser->add_argument(ArgumentSpec("n"));
           expect(
               [&] {
                 parser->add_argument(ArgumentSpec("name").set_short_name("n"));
               },
               throwsA<runtime_error>());
         });

    test("Registering an argument with the same short name as an existing "
         "one's short name throws",
         [&] {
           parser->add_argument(ArgumentSpec("name").set_short_name("n"));
           expect(
               [&] {
                 parser->add_argument(
                     ArgumentSpec("name2").set_short_name("n"));
               },
               throwsA<runtime_error>());
         });

    test("Registering an argument with a short name that is longer than "
         "one character throws",
         [&] {
           expect(
               [&] {
                 parser->add_argument(
                     ArgumentSpec("name").set_short_name("nnn"));
               },
               throwsA<runtime_error>());
         });
  });

  group("Non-existent default/implicit values", [&] {
    test("Not providing value for no-default argument", [&] {
      auto noDefaultArg =
          parser->add_argument(ArgumentSpec("x").set_implicit_value("i"));
      expect(
          [&] {
            parser->parse({});
          },
          throwsA<invalid_argument>());
    });

    test("Setting implicit value for no-implicit argument", [&] {
      auto noImplicitArg = parser->add_argument(ArgumentSpec("x"));
      expect(
          [&] {
            parser->parse({"--x"});
          },
          throwsA<invalid_argument>());
    });
  });
}
