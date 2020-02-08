#include <mcga/cli/numeric_argument.hpp>

namespace mcga::cli {

NumericArgumentSpec::NumericArgumentSpec(std::string name_)
    : name(std::move(name_)) {}

NumericArgumentSpec&
    NumericArgumentSpec::set_description(std::string description_) {
  description = std::move(description_);
  return *this;
}

NumericArgumentSpec&
    NumericArgumentSpec::set_help_group(std::string help_group_) {
  help_group = std::move(help_group_);
  return *this;
}

NumericArgumentSpec&
    NumericArgumentSpec::set_short_name(std::string short_name_) {
  short_name = std::move(short_name_);
  return *this;
}

NumericArgumentSpec&
    NumericArgumentSpec::set_default_value(const std::string& default_value_) {
  default_value.emplace(
      [default_value_]() {
        return default_value_;
      },
      default_value_);
  return *this;
}

NumericArgumentSpec& NumericArgumentSpec::set_default_value_generator(
    const std::function<std::string()>& default_value_gen,
    const std::string& default_value_desc) {
  default_value.emplace(default_value_gen, default_value_desc);
  return *this;
}

NumericArgumentSpec& NumericArgumentSpec::set_implicit_value(
    const std::string& implicit_value_) {
  implicit_value.emplace(
      [implicit_value_]() {
        return implicit_value_;
      },
      implicit_value_);
  return *this;
}

NumericArgumentSpec& NumericArgumentSpec::set_implicit_value_generator(
    const std::function<std::string()>& implicit_value_gen,
    const std::string& implicit_value_desc) {
  implicit_value.emplace(implicit_value_gen, implicit_value_desc);
  return *this;
}

namespace internal {

template<>
char from_string(const std::string& value) {
  return static_cast<char>(std::stoi(value));
}

template<>
unsigned char from_string(const std::string& value) {
  return static_cast<unsigned char>(std::stoul(value));
}

template<>
short int from_string(const std::string& value) {
  return static_cast<short int>(std::stoi(value));
}

template<>
unsigned short int from_string(const std::string& value) {
  return static_cast<unsigned short int>(std::stoul(value));
}

template<>
int from_string(const std::string& value) {
  return std::stoi(value);
}

template<>
unsigned int from_string(const std::string& value) {
  return static_cast<unsigned int>(std::stoul(value));
}

template<>
long from_string(const std::string& value) {
  return std::stol(value);
}

template<>
unsigned long from_string(const std::string& value) {
  return std::stoul(value);
}

template<>
long long from_string(const std::string& value) {
  return std::stoll(value);
}

template<>
unsigned long long from_string(const std::string& value) {
  return std::stoull(value);
}

template<>
float from_string(const std::string& value) {
  return std::stof(value);
}

template<>
double from_string(const std::string& value) {
  return std::stod(value);
}

template<>
long double from_string(const std::string& value) {
  return std::stold(value);
}

} // namespace internal

} // namespace mcga::cli