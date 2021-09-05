#pragma once

#include "command_line_option.hpp"
#include "disallow_copy_and_move.hpp"
#include "generator.hpp"
#include <memory>
#include <numeric>
#include <optional>
#include <string>

namespace mcga::cli {

namespace internal {

inline std::string join_strings(const std::vector<std::string>& parts) {
  if (parts.empty()) {
    return "";
  }

  std::string joined;
  joined.reserve(std::accumulate(parts.begin(), parts.end(), 0ul,
                                 [](auto partial_result, const auto& part) {
                                   return partial_result + part.size();
                                 }) +
                 joined.size() - 1);
  for (const auto& part: parts) {
    if (joined.size() != 0) {
      joined += ',';
    }
    joined += part;
  }
  return joined;
}

} // namespace internal

class Parser;

template<typename EArg = Argument>
struct ListArgumentSpec {
  std::string name;
  std::string description = "";
  std::string help_group = "";
  std::string short_name = "";
  std::optional<internal::ListGenerator> default_value;
  std::optional<internal::ListGenerator> implicit_value;

  explicit ListArgumentSpec(std::string name): name(std::move(name)) {}

  ListArgumentSpec& set_description(std::string description_) {
    description = std::move(description_);
    return *this;
  }

  ListArgumentSpec& set_help_group(std::string help_group_) {
    help_group = std::move(help_group_);
    return *this;
  }

  ListArgumentSpec& set_short_name(std::string short_name_) {
    short_name = std::move(short_name_);
    return *this;
  }

  ListArgumentSpec&
      set_default_value(const std::vector<std::string>& default_value_) {
    return set_default_value_generator(
        [default_value_]() {
          return default_value_;
        },
        internal::join_strings(default_value_));
  }

  ListArgumentSpec& set_default_value_generator(
      const std::function<std::vector<std::string>()>& default_value_gen,
      const std::string& default_value_desc = "<no description>") {
    default_value.emplace(default_value_gen, default_value_desc);
    return *this;
  }

  ListArgumentSpec&
      set_implicit_value(const std::vector<std::string>& implicit_value_) {
    return set_implicit_value_generator(
        [implicit_value_]() {
          return implicit_value_;
        },
        internal::join_strings(implicit_value_));
  }

  ListArgumentSpec& set_implicit_value_generator(
      const std::function<std::vector<std::string>()>& implicit_value_gen,
      const std::string& implicit_value_desc = "<no description>") {
    implicit_value.emplace(implicit_value_gen, implicit_value_desc);
    return *this;
  }
};

namespace internal {

template<typename EArg = Argument>
class ListArgumentImpl: public CommandLineOption {
using ValueType = typename EArg::ValueType;
using SpecType = typename EArg::SpecType;
using EltImpl = typename EArg::element_type;

public:
  explicit ListArgumentImpl(const ListArgumentSpec<EArg>& spec)
      : CommandLineOption(spec.default_value.has_value(),
                          spec.implicit_value.has_value()),
        spec(spec), impl(SpecType(spec.name)) {
  }

  ~ListArgumentImpl() override = default;

  [[nodiscard]] std::vector<ValueType> get_value() const {
    return value;
  }

  [[nodiscard]] const ListArgumentSpec<EArg>& get_spec() const {
    return spec;
  }

private:
  MCGA_DISALLOW_COPY_AND_MOVE(ListArgumentImpl);

  [[nodiscard]] const std::string& get_name() const override {
    return spec.name;
  }

  void reset() override {
    CommandLineOption::reset();
    applied_implicit = false;
    value.clear();
  }

  void set_default() override {
    value.clear();
    for (const std::string& val : spec.default_value.value().generate()) {
      impl.set_value(val);
      value.push_back(impl.get_value());
    }
  }

  void set_implicit() override {
    if (!applied_implicit) {
      for (const std::string& val : spec.implicit_value.value().generate()) {
        impl.set_value(val);
        value.push_back(impl.get_value());
      }
      applied_implicit = true;
    }
  }

  void set_value(const std::string& value_) override {
    impl.set_value(value_);
    value.push_back(impl.get_value());
  }

  bool applied_implicit = false;
  ListArgumentSpec<EArg> spec;
  std::vector<ValueType> value;
  EltImpl impl;

    friend class mcga::cli::Parser;
};

template<typename EArg = Argument>
using ListArgumentBase = std::shared_ptr<internal::ListArgumentImpl<EArg>>;

} // namespace internal

template<typename EArg = Argument>
struct ListArgument: internal::ListArgumentBase<EArg> {
  using ValueType = std::vector<typename EArg::ValueType>;
  using SpecType  = ListArgumentSpec<EArg>;
  using internal::ListArgumentBase<EArg>::ListArgumentBase;
  explicit ListArgument(internal::ListArgumentBase<EArg> ptr)
      : internal::ListArgumentBase<EArg>(std::move(ptr)) {}
};

} // namespace mcga::cli
