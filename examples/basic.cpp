#include <iostream>

#include <mcga/cli.hpp>

using mcga::cli::Argument;
using mcga::cli::ArgumentSpec;
using mcga::cli::Parser;
using std::boolalpha;
using std::cout;

int main(int argc, char** argv) {
  Parser parser("Basic example.");
  Argument arg = parser.add_argument(ArgumentSpec("arg")
                                         .set_default_value("default")
                                         .set_implicit_value("implicit")
                                         .set_description("A basic argument."));
  parser.parse(argc, argv);

  cout << "Argument appeared = " << boolalpha << arg->appeared() << "\n";
  cout << "Argument value = " << arg->get_value() << "\n";
  return 0;
}
