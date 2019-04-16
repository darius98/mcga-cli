#include <iostream>

#include <mcga/cli.hpp>

using mcga::cli::Argument;
using mcga::cli::ArgumentSpec;
using mcga::cli::Parser;
using std::boolalpha;
using std::cout;

int main(int argc, char** argv) {
    Parser parser("Basic example.");
    Argument arg = parser.addArgument(ArgumentSpec("arg")
                                        .setDefaultValue("default")
                                        .setImplicitValue("implicit")
                                        .setDescription("A basic argument."));
    parser.parse(argc, argv);

    cout << "Argument appeared = " << boolalpha << arg->appeared() << "\n";
    cout << "Argument value = " << arg->getValue() << "\n";
    return 0;
}
