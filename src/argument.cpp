#include <mcga/cli/argument.hpp>

namespace mcga::cli {

ArgumentSpec::ArgumentSpec(std::string name): name(std::move(name)) {}

ArgumentSpec& ArgumentSpec::set_description(std::string description_) {
  description = std::move(description_);
  return *this;
}

ArgumentSpec& ArgumentSpec::set_help_group(std::string help_group_) {
  help_group = std::move(help_group_);
  return *this;
}

ArgumentSpec& ArgumentSpec::set_short_name(std::string short_name_) {
  short_name = std::move(short_name_);
  return *this;
}

ArgumentSpec&
    ArgumentSpec::set_default_value(const std::string& default_value_) {
  default_value.emplace(
      [default_value_]() {
        return default_value_;
      },
      default_value_);
  return *this;
}

ArgumentSpec& ArgumentSpec::set_default_value_generator(
    const std::function<std::string()>& default_value_gen,
    const std::string& default_value_desc) {
  default_value.emplace(default_value_gen, default_value_desc);
  return *this;
}

ArgumentSpec&
    ArgumentSpec::set_implicit_value(const std::string& implicit_value_) {
  implicit_value.emplace(
      [implicit_value_]() {
        return implicit_value_;
      },
      implicit_value_);
  return *this;
}

ArgumentSpec& ArgumentSpec::set_implicit_value_generator(
    const std::function<std::string()>& implicit_value_gen,
    const std::string& implicit_value_desc) {
  implicit_value.emplace(implicit_value_gen, implicit_value_desc);
  return *this;
}

namespace internal {

std::string ArgumentImpl::get_value() const {
  return value;
}

const ArgumentSpec& ArgumentImpl::get_spec() const {
  return spec;
}

ArgumentImpl::ArgumentImpl(const ArgumentSpec& spec)
    : CommandLineSpec(spec.default_value.has_value(),
                      spec.implicit_value.has_value()),
      spec(spec) {}

const std::string& ArgumentImpl::get_name() const {
  return spec.name;
}

void ArgumentImpl::set_default() {
  value = spec.default_value.value().generate();
}

void ArgumentImpl::set_implicit() {
  value = spec.implicit_value.value().generate();
}

void ArgumentImpl::set_value(const std::string& value_) {
  value = value_;
}

} // namespace internal

} // namespace mcga::cli
