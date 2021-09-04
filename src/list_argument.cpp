#include <mcga/cli/list_argument.hpp>

#include <numeric>

namespace {

std::string join_strings(const std::vector<std::string>& parts) {
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

} // namespace

namespace mcga::cli {

ListArgumentSpec::ListArgumentSpec(std::string name): name(std::move(name)) {}

ListArgumentSpec& ListArgumentSpec::set_description(std::string description_) {
  description = std::move(description_);
  return *this;
}

ListArgumentSpec& ListArgumentSpec::set_help_group(std::string help_group_) {
  help_group = std::move(help_group_);
  return *this;
}

ListArgumentSpec& ListArgumentSpec::set_short_name(std::string short_name_) {
  short_name = std::move(short_name_);
  return *this;
}

ListArgumentSpec& ListArgumentSpec::set_default_value(
    const std::vector<std::string>& default_value_) {
  return set_default_value_generator(
      [default_value_]() {
        return default_value_;
      },
      join_strings(default_value_));
}
ListArgumentSpec& ListArgumentSpec::set_default_value_generator(
    const std::function<std::vector<std::string>()>& default_value_gen,
    const std::string& default_value_desc) {
  default_value.emplace(default_value_gen, default_value_desc);
  return *this;
}

ListArgumentSpec& ListArgumentSpec::set_implicit_value(
    const std::vector<std::string>& implicit_value_) {
  return set_implicit_value_generator(
      [implicit_value_]() {
        return implicit_value_;
      },
      join_strings(implicit_value_));
}

ListArgumentSpec& ListArgumentSpec::set_implicit_value_generator(
    const std::function<std::vector<std::string>()>& implicit_value_gen,
    const std::string& implicit_value_desc) {
  implicit_value.emplace(implicit_value_gen, implicit_value_desc);
  return *this;
}

namespace internal {

ListArgumentImpl::ListArgumentImpl(const ListArgumentSpec& spec)
    : CommandLineOption(spec.default_value.has_value(),
                      spec.implicit_value.has_value()),
      spec(spec) {}

std::vector<std::string> ListArgumentImpl::get_value() const {
  return value;
}

const ListArgumentSpec& ListArgumentImpl::get_spec() const {
  return spec;
}

const std::string& ListArgumentImpl::get_name() const {
  return spec.name;
}

void ListArgumentImpl::reset() {
  CommandLineOption::reset();
  applied_implicit = false;
  value.clear();
}

void ListArgumentImpl::set_default() {
  value = spec.default_value.value().generate();
}

void ListArgumentImpl::set_implicit() {
  if (!applied_implicit) {
    auto implicit_value = spec.implicit_value.value().generate();
    value.insert(value.begin(), implicit_value.begin(), implicit_value.end());
    applied_implicit = true;
  }
}

void ListArgumentImpl::set_value(const std::string& value_) {
  value.push_back(value_);
}

} // namespace internal

} // namespace mcga::cli
