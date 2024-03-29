#pragma once

#include <memory>
#include <optional>
#include <string>

#include "command_line_option.hpp"
#include "disallow_copy_and_move.hpp"
#include "generator.hpp"

namespace mcga::cli {

class Parser;

struct ArgumentSpec {
  std::string name;
  std::string description;
  std::string help_group;
  std::string short_name;
  std::optional<internal::Generator> default_value;
  std::optional<internal::Generator> implicit_value;

  explicit ArgumentSpec(std::string name);

  ArgumentSpec& set_description(std::string description_);
  ArgumentSpec& set_help_group(std::string help_group_);
  ArgumentSpec& set_short_name(std::string short_name_);

  ArgumentSpec& set_default_value(const std::string& default_value_);
  ArgumentSpec& set_default_value_generator(
      const std::function<std::string()>& default_value_gen,
      const std::string& default_value_desc = "<no description>");

  ArgumentSpec& set_implicit_value(const std::string& implicit_value_);
  ArgumentSpec& set_implicit_value_generator(
      const std::function<std::string()>& implicit_value_gen,
      const std::string& implicit_value_desc = "<no description>");
};

namespace internal {

class ArgumentImpl: public CommandLineOption {
public:
  explicit ArgumentImpl(const ArgumentSpec& spec);

  ~ArgumentImpl() override = default;

  [[nodiscard]] std::optional<std::string> get_value_if_exists() const;

  [[nodiscard]] std::string get_value() const;

  [[nodiscard]] const ArgumentSpec& get_spec() const;

private:
  MCGA_DISALLOW_COPY_AND_MOVE(ArgumentImpl);

  [[nodiscard]] const std::string& get_name() const override;

  void set_default() override;

  void set_implicit() override;

  void set_value(const std::string& value_) override;

  ArgumentSpec spec;
  std::string value;

  friend class mcga::cli::Parser;
  template<typename EArg>
  friend class ListArgumentImpl;
};

using ArgumentBase = std::shared_ptr<ArgumentImpl>;

} // namespace internal

struct Argument: internal::ArgumentBase {
  using ValueType = std::string;
  using SpecType = ArgumentSpec;

  using internal::ArgumentBase::ArgumentBase;
  explicit Argument(internal::ArgumentBase ptr)
      : internal::ArgumentBase(std::move(ptr)) {}
};

} // namespace mcga::cli
