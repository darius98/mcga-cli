#pragma once

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "argument.hpp"
#include "choice_argument.hpp"
#include "command_line_spec.hpp"
#include "flag.hpp"
#include "numeric_argument.hpp"

namespace mcga::cli {

class Parser {
public:
  using ArgList = std::vector<std::string>;

  explicit Parser(const std::string& help_prefix_);

  MCGA_DISALLOW_COPY_AND_MOVE(Parser);

  ~Parser() = default;

  Argument add_argument(const ArgumentSpec& spec);

  template<class T>
  NumericArgument<T> add_numeric_argument(const NumericArgumentSpec& spec) {
    check_name_availability(spec.name, spec.short_name);
    auto argument = std::make_shared<internal::NumericArgumentImpl<T>>(spec);
    add_spec(argument, spec.name, spec.short_name);
    add_numeric_argument_help(spec.default_value, spec.implicit_value,
                              spec.help_group, spec.name, spec.short_name,
                              spec.description);
    return argument;
  }

  Flag add_flag(const FlagSpec& spec);
  void add_terminal_flag(const FlagSpec& spec,
                         const std::function<void()>& callback);
  void add_terminal_flag(const FlagSpec& spec, const std::string& message);
  void add_help_flag();

  template<class T>
  ChoiceArgument<T> add_choice_argument(const ChoiceArgumentSpec<T>& spec) {
    check_name_availability(spec.name, spec.short_name);
    auto choice_argument =
        std::make_shared<internal::ChoiceArgumentImpl<T>>(spec);
    add_spec(choice_argument, spec.name, spec.short_name);
    std::string rendered_options;
    bool first = true;
    for (const auto& option: spec.options) {
      if (!first) {
        rendered_options += ",";
      }
      first = false;
      rendered_options += "'" + option.first + "'";
    }
    rendered_options = "[" + rendered_options + "]";
    add_choice_argument_help(spec.default_value, spec.implicit_value,
                             spec.help_group, spec.name, spec.short_name,
                             spec.description, rendered_options);
    return choice_argument;
  }

  ArgList parse(const ArgList& args);
  ArgList parse(int argc, char** argv);
  [[nodiscard]] std::string render_help() const;

private:
  using CommandLineSpecPtr = std::shared_ptr<internal::CommandLineSpec>;

  struct HelpGroup {
    std::string group_name;
    std::string content;
  };

  void add_spec(const CommandLineSpecPtr& spec, const std::string& name,
                const std::string& short_name);

  void apply_value(const std::string& cliString, const std::string& value);

  void apply_implicit(const std::string& cliString);

  [[nodiscard]] bool should_apply_value(const std::string& cliString) const;

  void check_name_availability(const std::string& name,
                               const std::string& short_name) const;

  void add_choice_argument_help(
      const std::optional<internal::Generator>& default_value,
      const std::optional<internal::Generator>& implicit_value,
      const std::string& help_group, const std::string& name,
      const std::string& short_name, const std::string& description,
      const std::string& rendered_options);

  void add_numeric_argument_help(
      const std::optional<internal::Generator>& default_value,
      const std::optional<internal::Generator>& implicit_value,
      const std::string& help_group, const std::string& name,
      const std::string& short_name, const std::string& description);

  void add_help(const std::string& help_group, const std::string& name,
                const std::string& short_name, const std::string& description,
                const std::string& extra);

  template<class T>
  static std::string to_string(const T& value) {
    return std::to_string(value);
  }

  std::vector<CommandLineSpecPtr> specs;
  std::map<std::string, CommandLineSpecPtr> specs_by_cli_string;

  std::string help_prefix;
  std::vector<HelpGroup> help_sections;

  std::set<std::string> reserved_names;

  std::vector<std::pair<Flag, std::function<void()>>> terminal_flags;
};

template<>
std::string Parser::to_string(const std::string& value);

} // namespace mcga::cli
