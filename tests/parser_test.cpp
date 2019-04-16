#include <kktest.hpp>
#include <kktest_ext/matchers.hpp>

#include <mcga/cli.hpp>

using kktest::group;
using kktest::setUp;
using kktest::tearDown;
using kktest::test;
using kktest::matchers::isEqualTo;
using kktest::matchers::throwsA;
using mcga::cli::Argument;
using mcga::cli::ArgumentSpec;
using mcga::cli::Parser;
using std::runtime_error;
using std::string;
using std::vector;

TEST_CASE(McgaCliParser, "Parser") {
    Parser* parser = nullptr;

    setUp([&] { parser = new Parser("Help prefix."); });

    tearDown([&] {
        delete parser;
        parser = nullptr;
    });

    group("Single argument", [&] {
        Argument arg;

        setUp([&] {
            arg = parser->addArgument(ArgumentSpec("name")
                                        .setShortName("n")
                                        .setDefaultValue("a")
                                        .setImplicitValue("b"));
        });

        test("no value provided leads to argument taking default value", [&] {
            parser->parse({});
            expect(arg->getValue(), isEqualTo("a"));
        });

        test("provided with single dash, short name", [&] {
            parser->parse({"-n"});
            expect(arg->getValue(), isEqualTo("b"));
        });

        test("provided with double dash, short name", [&] {
            parser->parse({"--n"});
            expect(arg->getValue(), isEqualTo("b"));
        });

        test("provided with double dash, long name", [&] {
            parser->parse({"--name"});
            expect(arg->getValue(), isEqualTo("b"));
        });

        test("value provided with single dash & space", [&] {
            parser->parse({"-n", "v"});
            expect(arg->getValue(), isEqualTo("v"));
        });

        test("value provided with single dash & equal sign", [&] {
            parser->parse({"-n=v"});
            expect(arg->getValue(), isEqualTo("v"));
        });

        test("value provided with double dash & space is positional, while "
             "the argument takes implicit value",
             [&] {
                 auto positionalArgs = parser->parse({"--name", "v"});
                 expect(arg->getValue(), isEqualTo("b"));
                 expect(positionalArgs, isEqualTo(vector<string>{"v"}));
             });

        test("value provided with double dash & equal sign (short name)", [&] {
            parser->parse({"--n=v"});
            expect(arg->getValue(), isEqualTo("v"));
        });

        test("value provided with double dash & equal sign (long name)", [&] {
            parser->parse({"--name=v"});
            expect(arg->getValue(), isEqualTo("v"));
        });

        test("providing value for different argument name does not influence"
             " interesting argument",
             [&] {
                 parser->parse({"-m", "v"});
                 expect(arg->getValue(), isEqualTo("a"));
             });

        test("when providing multiple values for one argument, it takes the "
             "last one",
             [&] {
                 parser->parse({"-n", "v1", "-n", "--name=v2"});
                 expect(arg->getValue(), isEqualTo("v2"));
             });
    });

    group("Multiple arguments", [&] {
        Argument a;
        Argument b;
        Argument c;

        setUp([&] {
            a = parser->addArgument(ArgumentSpec("arg_a")
                                      .setShortName("a")
                                      .setDefaultValue("default")
                                      .setImplicitValue("implicit"));
            b = parser->addArgument(ArgumentSpec("arg_b")
                                      .setShortName("b")
                                      .setDefaultValue("default")
                                      .setImplicitValue("implicit"));
            c = parser->addArgument(ArgumentSpec("arg_c")
                                      .setShortName("c")
                                      .setDefaultValue("default")
                                      .setImplicitValue("implicit"));
        });

        test("Providing values for multiple arguments via double dash", [&] {
            parser->parse({"--arg_a=value", "--b"});
            expect(a->getValue(), isEqualTo("value"));
            expect(b->getValue(), isEqualTo("implicit"));
            expect(c->getValue(), isEqualTo("default"));
        });

        test("Providing values for multiple arguments with multiple single "
             "dash arguments",
             [&] {
                 parser->parse({"-a", "-b", "value"});
                 expect(a->getValue(), isEqualTo("implicit"));
                 expect(b->getValue(), isEqualTo("value"));
                 expect(c->getValue(), isEqualTo("default"));
             });

        test("Providing implicit values for multiple arguments via a single"
             " dash argument",
             [&] {
                 parser->parse({"-ab"});
                 expect(a->getValue(), isEqualTo("implicit"));
                 expect(b->getValue(), isEqualTo("implicit"));
                 expect(c->getValue(), isEqualTo("default"));
             });

        test("Providing values for multiple arguments via a single dash"
             "argument & space for non-implicit value of the last one",
             [&] {
                 parser->parse({"-abc", "value"});
                 expect(a->getValue(), isEqualTo("implicit"));
                 expect(b->getValue(), isEqualTo("implicit"));
                 expect(c->getValue(), isEqualTo("value"));
             });

        test("Providing values for multiple arguments via a single dash"
             "argument & equal sign for non-implicit value of the last one",
             [&] {
                 parser->parse({"-abc=value"});
                 expect(a->getValue(), isEqualTo("implicit"));
                 expect(b->getValue(), isEqualTo("implicit"));
                 expect(c->getValue(), isEqualTo("value"));
             });
    });

    group("Invalid argument names", [&] {
        test("Registering an argument with the same name as an existing one "
             "throws",
             [&] {
                 parser->addArgument(ArgumentSpec("name"));
                 expect([&] { parser->addArgument(ArgumentSpec("name")); },
                        throwsA<runtime_error>());
             });

        test("Registering an argument with the same name as an existing one's "
             "short name throws",
             [&] {
                 parser->addArgument(ArgumentSpec("name").setShortName("n"));
                 expect([&] { parser->addArgument(ArgumentSpec("n")); },
                        throwsA<runtime_error>());
             });

        test("Registering an argument with the same short name as an existing "
             "one's name throws",
             [&] {
                 parser->addArgument(ArgumentSpec("n"));
                 expect(
                   [&] {
                       parser->addArgument(
                         ArgumentSpec("name").setShortName("n"));
                   },
                   throwsA<runtime_error>());
             });

        test("Registering an argument with the same short name as an existing "
             "one's short name throws",
             [&] {
                 parser->addArgument(ArgumentSpec("name").setShortName("n"));
                 expect(
                   [&] {
                       parser->addArgument(
                         ArgumentSpec("name2").setShortName("n"));
                   },
                   throwsA<runtime_error>());
             });

        test("Registering an argument with a short name that is longer than "
             "one character throws",
             [&] {
                 expect(
                   [&] {
                       parser->addArgument(
                         ArgumentSpec("name").setShortName("nnn"));
                   },
                   throwsA<runtime_error>());
             });
    });
}
