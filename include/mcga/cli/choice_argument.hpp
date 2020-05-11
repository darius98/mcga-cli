#pragma once

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "command_line_spec.hpp"
#include "disallow_copy_and_move.hpp"
#include "exceptions.hpp"
#include "generator.hpp"

namespace mcga::cli {

template<class T>
struct ChoiceArgumentSpec {
  std::string name;
  std::string description = "";
  std::string help_group = "";
  std::string short_name = "";
  std::map<std::string, T> options;
  std::optional<internal::Generator> default_value;
  std::optional<internal::Generator> implicit_value;

  explicit ChoiceArgumentSpec(std::string name_): name(std::move(name_)) {}

  ChoiceArgumentSpec& set_short_name(const std::string& short_name_) {
    short_name = short_name_;
    return *this;
  }

  ChoiceArgumentSpec& set_description(const std::string& description_) {
    description = description_;
    return *this;
  }

  ChoiceArgumentSpec& set_help_group(const std::string& help_group_) {
    help_group = help_group_;
    return *this;
  }

  ChoiceArgumentSpec& set_default_value(const std::string& default_value_) {
    default_value.emplace(
        [default_value_]() {
          return default_value_;
        },
        default_value_);
    return *this;
  }

  ChoiceArgumentSpec& set_default_value_generator(
      const std::function<std::string()>& default_value_gen,
      const std::string& default_value_desc = "<no description>") {
    default_value.emplace(default_value_gen, default_value_desc);
    return *this;
  }

  ChoiceArgumentSpec& set_implicit_value(const std::string& implicit_value_) {
    implicit_value.emplace(
        [implicit_value_]() {
          return implicit_value_;
        },
        implicit_value_);
    return *this;
  }

  ChoiceArgumentSpec& set_implicit_value_generator(
      const std::function<std::string()>& implicit_value_gen,
      const std::string& implicit_value_desc = "<no description>") {
    implicit_value.emplace(implicit_value_gen, implicit_value_desc);
    return *this;
  }

  ChoiceArgumentSpec&
      set_options(const std::vector<std::pair<std::string, T>>& options_) {
    options.clear();
    return add_options(options_);
  }

  ChoiceArgumentSpec&
      add_options(const std::vector<std::pair<std::string, T>>& extra_options) {
    for (const std::pair<std::string, T>& option: extra_options) {
      options[option.first] = option.second;
    }
    return *this;
  }

  ChoiceArgumentSpec& add_option(const std::string& key, const T& value) {
    options[key] = value;
    return *this;
  }
};

namespace internal {

template<class T>
class ChoiceArgumentImpl: public CommandLineSpec {
public:
  explicit ChoiceArgumentImpl(const ChoiceArgumentSpec<T>& spec)
      : CommandLineSpec(spec.default_value.has_value(),
                        spec.implicit_value.has_value()),
        spec(spec) {}

  ~ChoiceArgumentImpl() override = default;

  const ChoiceArgumentSpec<T>& get_spec() const {
    return spec;
  }

  T get_value() const {
    return value;
  }

private:
  MCGA_DISALLOW_COPY_AND_MOVE(ChoiceArgumentImpl);

  [[nodiscard]] const std::string& get_name() const override {
    return spec.name;
  }

  void set_default() override {
    set_value(spec.default_value.value().generate());
  }

  void set_implicit() override {
    set_value(spec.implicit_value.value().generate());
  }

  void set_value(const std::string& value_) override {
    auto it = spec.options.find(value_);
    if (it != spec.options.end()) {
      value = it->second;
      return;
    }
    std::string rendered_options;
    bool first = true;
    for (const std::pair<std::string, T>& option: spec.options) {
      if (!first) {
        rendered_options += ",";
      }
      first = false;
      rendered_options += "'" + option.first + "'";
    }
    internal::throw_invalid_argument_exception(
        "Trying to set option `" + value_ + "` to argument " + spec.name +
        ", which has options [" + rendered_options + "]");
  }

  ChoiceArgumentSpec<T> spec;
  T value;

  friend class mcga::cli::Parser;
};

} // namespace internal

template<class T>
using ChoiceArgument = std::shared_ptr<internal::ChoiceArgumentImpl<T>>;

} // namespace mcga::cli
