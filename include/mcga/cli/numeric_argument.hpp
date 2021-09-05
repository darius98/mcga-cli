#pragma once

#include <memory>
#include <optional>
#include <string>
#include <type_traits>

#include "command_line_option.hpp"
#include "disallow_copy_and_move.hpp"
#include "generator.hpp"

namespace mcga::cli {

struct NumericArgumentSpec {
  std::string name;
  std::string description = "";
  std::string help_group = "";
  std::string short_name = "";
  std::optional<internal::Generator> default_value;
  std::optional<internal::Generator> implicit_value;

  explicit NumericArgumentSpec(std::string name_);
  NumericArgumentSpec& set_description(std::string description_);
  NumericArgumentSpec& set_help_group(std::string help_group_);
  NumericArgumentSpec& set_short_name(std::string short_name_);

  NumericArgumentSpec& set_default_value(const std::string& default_value_);
  NumericArgumentSpec& set_default_value_generator(
      const std::function<std::string()>& default_value_gen,
      const std::string& default_value_desc = "<no description>");

  NumericArgumentSpec& set_implicit_value(const std::string& implicit_value_);
  NumericArgumentSpec& set_implicit_value_generator(
      const std::function<std::string()>& implicit_value_gen,
      const std::string& implicit_value_desc = "<no description>");
};

namespace internal {

template<class T>
T from_string(const std::string& value);

template<>
char from_string(const std::string& value);

template<>
unsigned char from_string(const std::string& value);

template<>
short int from_string(const std::string& value);

template<>
unsigned short int from_string(const std::string& value);

template<>
int from_string(const std::string& value);

template<>
unsigned int from_string(const std::string& value);

template<>
long from_string(const std::string& value);

template<>
unsigned long from_string(const std::string& value);

template<>
long long from_string(const std::string& value);

template<>
unsigned long long from_string(const std::string& value);

template<>
float from_string(const std::string& value);

template<>
double from_string(const std::string& value);

template<>
long double from_string(const std::string& value);

template<class T>
class NumericArgumentImpl: public CommandLineOption {
public:
  explicit NumericArgumentImpl(const NumericArgumentSpec& spec)
      : CommandLineOption(spec.default_value.has_value(),
                          spec.implicit_value.has_value()),
        spec(spec) {}

  ~NumericArgumentImpl() override = default;

  [[nodiscard]] T get_value() const {
    return value;
  }

private:
  MCGA_DISALLOW_COPY_AND_MOVE(NumericArgumentImpl);

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
    value = from_string<T>(value_);
  }

  NumericArgumentSpec spec;
  T value;

  friend class mcga::cli::Parser;
};

template<class T>
using NumericArgumentBase = std::shared_ptr<NumericArgumentImpl<T>>;

} // namespace internal

template<class T>
struct NumericArgument: internal::NumericArgumentBase<T> {
  using ValueType = T;

  using internal::NumericArgumentBase<T>::NumericArgumentBase;
  explicit NumericArgument(internal::NumericArgumentBase<T> ptr)
      : internal::NumericArgumentBase<T>(std::move(ptr)) {}
};

} // namespace mcga::cli
