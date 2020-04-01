#include <mcga/cli/command_line_spec.hpp>

#include <mcga/cli/exceptions.hpp>

namespace mcga::cli::internal {

bool CommandLineSpec::appeared() const {
  return appeared_in_args;
}

CommandLineSpec::CommandLineSpec(bool has_default_value_,
                                 bool has_implicit_value_)
    : has_default_value(has_default_value_),
      has_implicit_value(has_implicit_value_) {}

bool CommandLineSpec::consumes_next_positional_arg() const {
  return true;
}

void CommandLineSpec::reset() {
  appeared_in_args = false;
}

void CommandLineSpec::set_default_guarded() {
  if (!has_default_value) {
    internal::throw_invalid_argument_exception(
        "Trying to set default value for argument " + get_name() +
        ", which has no default value.");
  }
  set_default();
  appeared_in_args = false;
}

void CommandLineSpec::set_implicit_guarded() {
  if (!has_implicit_value) {
    internal::throw_invalid_argument_exception(
        "Trying to set implicit value for argument " + get_name() +
        ", which has no implicit value.");
  }
  set_implicit();
  appeared_in_args = true;
}

void CommandLineSpec::set_value_guarded(const std::string& value) {
  set_value(value);
  appeared_in_args = true;
}

} // namespace mcga::cli::internal
