#include <mcga/cli/parser.hpp>

#include <iostream>

namespace mcga::cli {

Parser::Parser(const std::string& help_prefix_)
    : help_prefix(help_prefix_ + "\n") {}

Argument Parser::add_argument(const ArgumentSpec& spec) {
  check_name_availability(spec.name, spec.short_name);
  Argument argument = std::make_shared<internal::ArgumentImpl>(spec);
  add_spec(argument, spec.name, spec.short_name);
  std::string extra;
  if (spec.default_value.has_value() && spec.implicit_value.has_value()) {
    extra = "\t\tDefault: '" + spec.default_value.value().get_description() +
            "', Implicit: '" + spec.implicit_value.value().get_description() +
            "'";
  } else if (spec.default_value.has_value()) {
    extra =
        "\t\tDefault: '" + spec.default_value.value().get_description() + "'";
  } else if (spec.implicit_value.has_value()) {
    extra =
        "\t\tImplicit: '" + spec.implicit_value.value().get_description() + "'";
  }
  add_help(spec.help_group, spec.name, spec.short_name, spec.description,
           extra);
  return argument;
}

ListArgument Parser::add_list_argument(const ListArgumentSpec& spec) {
  check_name_availability(spec.name, spec.short_name);
  ListArgument argument = std::make_shared<internal::ListArgumentImpl>(spec);
  add_spec(argument, spec.name, spec.short_name);
  std::string extra;
  if (spec.default_value.has_value() && spec.implicit_value.has_value()) {
    extra = "\t\tDefault: '" + spec.default_value.value().get_description() +
            "', Implicit: '" + spec.implicit_value.value().get_description() +
            "'";
  } else if (spec.default_value.has_value()) {
    extra =
        "\t\tDefault: '" + spec.default_value.value().get_description() + "'";
  } else if (spec.implicit_value.has_value()) {
    extra =
        "\t\tImplicit: '" + spec.implicit_value.value().get_description() + "'";
  }
  add_help(spec.help_group, spec.name, spec.short_name, spec.description,
           extra);
  return argument;
}

Flag Parser::add_flag(const FlagSpec& spec) {
  check_name_availability(spec.name, spec.short_name);
  Flag flag = std::make_shared<internal::FlagImpl>(spec);
  add_spec(flag, spec.name, spec.short_name);
  add_help(spec.help_group, spec.name, spec.short_name,
           "(Flag)\t" + spec.description, "");
  return flag;
}

void Parser::add_terminal_flag(const FlagSpec& spec,
                               const std::function<void()>& callback) {
  terminal_flags.emplace_back(add_flag(spec), callback);
}

void Parser::add_terminal_flag(const FlagSpec& spec,
                               const std::string& message) {
  terminal_flags.emplace_back(add_flag(spec), [message] {
    std::cout << message;
  });
}

void Parser::add_help_flag() {
  add_terminal_flag(FlagSpec("help").set_short_name("h").set_description(
                        "Display this help menu."),
                    [this]() {
                      std::cout << render_help();
                    });
}

auto Parser::parse(const ArgList& args) -> ArgList {
  for (const CommandLineSpecPtr& spec: specs) {
    spec->reset();
  }

  ArgList positional_args;
  std::string last_short_name;
  bool only_positional = false;
  for (const std::string& arg: args) {
    // on encountering the "--" argument, all arguments from that point
    // on are considered positional.
    if (arg == "--") {
      only_positional = true;
      continue;
    }

    // all arguments after "--" are considered positional.
    if (only_positional) {
      positional_args.push_back(arg);
      continue;
    }

    // an argument that does not start with '-' (or is equal to "-") is
    // not considered special, and therefore will be treated like either
    // a positional argument or a value filler for the last unfulfilled
    // short name argument given in the format "-XYZ".
    if (arg.substr(0, 1) != "-" || arg == "-") {
      if (should_apply_value(last_short_name)) {
        // `last_short_name` is an unfulfilled argument given by short
        // name in the format "-XYZ v" as "Z".
        apply_value(last_short_name, arg);
        last_short_name = "";
      } else {
        // no unfulfilled argument given by short name, considering
        // a positional argument.
        positional_args.push_back(arg);
      }
      continue;
    }

    // if we reached this point without hitting a `continue`, now the
    // current argument is definitely a special one.

    // if we had an unfulfilled argument given by short name, we will
    // give it its implicit value since it won't be fulfilled by this
    // argument.
    if (!last_short_name.empty()) {
      apply_implicit(last_short_name);
      last_short_name = "";
    }

    auto equal_pos = arg.find('=');

    // 1. for "--X", give argument "X" its implicit value
    if (arg.substr(0, 2) == "--" && equal_pos == std::string::npos) {
      apply_implicit(arg.substr(2));
    }

    // 2. for "--X=v", give argument "X" value "v"
    if (arg.substr(0, 2) == "--" && equal_pos != std::string::npos) {
      apply_value(arg.substr(2, equal_pos - 2), arg.substr(equal_pos + 1));
    }

    // 3. for "-XYZ", give arguments "X" and "Y" their implicit values,
    // and remember "Z" as the last short name, as a construct of the
    // form "-XYZ v" is allowed, equivalent with "--X --Y --Z=v".
    if (arg.substr(0, 2) != "--" && equal_pos == std::string::npos) {
      for (size_t j = 1; j + 1 < arg.length(); ++j) {
        apply_implicit(arg.substr(j, 1));
      }
      last_short_name = arg.substr(arg.length() - 1, 1);
    }

    // 4. for "-XYZ=v", give arguments "X" and "Y" their implicit values
    // and argument "Z" value "v".
    if (arg.substr(0, 2) != "--" && equal_pos != std::string::npos) {
      for (size_t j = 1; j + 1 < equal_pos; ++j) {
        apply_implicit(arg.substr(j, 1));
      }
      apply_value(arg.substr(equal_pos - 1, 1), arg.substr(equal_pos + 1));
    }
  }
  if (!last_short_name.empty()) {
    apply_implicit(last_short_name);
  }

  for (const CommandLineSpecPtr& spec: specs) {
    if (!spec->appeared()) {
      spec->set_default_guarded();
    }
  }

  for (const auto& flag: terminal_flags) {
    if (flag.first->get_value()) {
      flag.second();
      exit(0);
    }
  }

  return positional_args;
}

auto Parser::parse(int argc, char** argv) -> ArgList {
  ArgList args;
  args.reserve(static_cast<std::size_t>(argc));
  for (int i = 0; i < argc; ++i) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    args.emplace_back(argv[i]);
  }
  return parse(args);
}

std::string Parser::render_help() const {
  std::string help = help_prefix + "\n";
  for (const HelpGroup& group: help_sections) {
    help += "\n" + group.content;
  }
  return help;
}

void Parser::add_spec(const CommandLineSpecPtr& spec, const std::string& name,
                      const std::string& short_name) {
  specs.push_back(spec);
  reserved_names.insert(name);
  specs_by_cli_string[name] = spec;
  if (!short_name.empty()) {
    reserved_names.insert(short_name);
    specs_by_cli_string[short_name] = spec;
  }
}

void Parser::apply_value(const std::string& cliString,
                         const std::string& value) {
  auto it = specs_by_cli_string.find(cliString);
  if (it != specs_by_cli_string.end()) {
    it->second->set_value_guarded(value);
  }
}

void Parser::apply_implicit(const std::string& cliString) {
  auto it = specs_by_cli_string.find(cliString);
  if (it != specs_by_cli_string.end()) {
    it->second->set_implicit_guarded();
  }
}

[[nodiscard]] bool
    Parser::should_apply_value(const std::string& cliString) const {
  auto it = specs_by_cli_string.find(cliString);
  return it != specs_by_cli_string.end() &&
         it->second->consumes_next_positional_arg();
}

void Parser::check_name_availability(const std::string& name,
                                     const std::string& short_name) const {
  if (reserved_names.count(name) != 0) {
    internal::throw_logic_error(
        "Argument tried to register " + name +
        " as a command-line "
        "name, but a different argument already has it as a name.");
  }
  if (!short_name.empty() && reserved_names.count(short_name) != 0) {
    internal::throw_logic_error(
        "Argument tried to register " + short_name +
        " as a command-line"
        " short name, but a different argument already has it as a "
        "short name.");
  }
  if (short_name.size() > 1) {
    internal::throw_logic_error(
        "Argument short name should always have length 1.");
  }
}

void Parser::add_choice_argument_help(
    const std::optional<internal::Generator>& default_value,
    const std::optional<internal::Generator>& implicit_value,
    const std::string& help_group, const std::string& name,
    const std::string& short_name, const std::string& description,
    const std::string& rendered_options) {
  std::string extra;
  if (default_value.has_value() && implicit_value.has_value()) {
    extra = "\t\tDefault: '" + default_value.value().get_description() +
            "', Implicit: '" + implicit_value.value().get_description() +
            "', allowed values: " + rendered_options;
  } else if (default_value.has_value()) {
    extra = "\t\tDefault: '" + default_value.value().get_description() +
            "', allowed values: " + rendered_options;
  } else if (implicit_value.has_value()) {
    extra = "\t\tImplicit: '" + implicit_value.value().get_description() +
            "', allowed values: " + rendered_options;
  }
  add_help(help_group, name, short_name, description, extra);
}

void Parser::add_numeric_argument_help(
    const std::optional<internal::Generator>& default_value,
    const std::optional<internal::Generator>& implicit_value,
    const std::string& help_group, const std::string& name,
    const std::string& short_name, const std::string& description) {
  std::string extra;
  if (default_value.has_value() && implicit_value.has_value()) {
    extra = "\t\tDefault: '" + default_value.value().get_description() +
            "', Implicit: '" + implicit_value.value().get_description() + "'";
  } else if (default_value.has_value()) {
    extra = "\t\tDefault: '" + default_value.value().get_description() + "'";
  } else if (implicit_value.has_value()) {
    extra = "\t\tImplicit: '" + implicit_value.value().get_description() + "'";
  }
  add_help(help_group, name, short_name, "(Number)\t" + description, extra);
}

void Parser::add_help(const std::string& help_group, const std::string& name,
                      const std::string& short_name,
                      const std::string& description,
                      const std::string& extra) {
  std::string help_line = "\t--" + name;
  if (!short_name.empty()) {
    help_line += ",-" + short_name;
  }
  help_line += "\t" + description;
  if (!extra.empty()) {
    help_line += "\n" + extra;
  }

  if (help_group.empty()) {
    help_prefix += "\n" + help_line;
    return;
  }

  bool found_help_group = false;
  for (HelpGroup& group: help_sections) {
    if (group.group_name == help_group) {
      found_help_group = true;
      group.content += help_line + "\n";
      break;
    }
  }
  if (!found_help_group) {
    help_sections.push_back({help_group, help_group + "\n" + help_line + "\n"});
  }
}

template<>
std::string Parser::to_string(const std::string& value) {
  return value;
}

} // namespace mcga::cli