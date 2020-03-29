#pragma once

#include <optional>
#include <string>

#include "command_line_spec.hpp"
#include "disallow_copy_and_move.hpp"
#include "generator.hpp"

namespace mcga::cli {

class Parser;

struct ListArgumentSpec {
  std::string name;
  std::string description = "";
  std::string help_group = "";
  std::string short_name = "";
  std::optional<internal::ListGenerator> default_value;
  std::optional<internal::ListGenerator> implicit_value;

  explicit ListArgumentSpec(std::string name);

  ListArgumentSpec& set_description(std::string description_);
  ListArgumentSpec& set_help_group(std::string help_group_);
  ListArgumentSpec& set_short_name(std::string short_name_);

  ListArgumentSpec&
      set_default_value(const std::vector<std::string>& default_value_);

  ListArgumentSpec& set_default_value_generator(
      const std::function<std::vector<std::string>()>& default_value_gen,
      const std::string& default_value_desc = "<no description>");

  ListArgumentSpec&
      set_implicit_value(const std::vector<std::string>& implicit_value_);

  ListArgumentSpec& set_implicit_value_generator(
      const std::function<std::vector<std::string>()>& implicit_value_gen,
      const std::string& implicit_value_desc = "<no description>");
};

namespace internal {

class ListArgumentImpl: public CommandLineSpec {
public:
  explicit ListArgumentImpl(const ListArgumentSpec& spec);

  ~ListArgumentImpl() override = default;

  [[nodiscard]] std::vector<std::string> get_value() const;

  [[nodiscard]] const ListArgumentSpec& get_spec() const;

private:
  MCGA_DISALLOW_COPY_AND_MOVE(ListArgumentImpl);

  [[nodiscard]] const std::string& get_name() const override;

  void reset() override;

  void set_default() override;

  void set_implicit() override;

  void set_value(const std::string& value_) override;

  bool applied_implicit = false;
  ListArgumentSpec spec;
  std::vector<std::string> value;

  friend class mcga::cli::Parser;
};

} // namespace internal

using ListArgument = std::shared_ptr<internal::ListArgumentImpl>;

} // namespace mcga::cli
