#include <mcga/cli/flag.hpp>

namespace mcga::cli {

FlagSpec::FlagSpec(std::string name_): name(std::move(name_)) {}

FlagSpec& FlagSpec::set_description(std::string description_) {
  description = std::move(description_);
  return *this;
}

FlagSpec& FlagSpec::set_help_group(std::string help_group_) {
  help_group = std::move(help_group_);
  return *this;
}

FlagSpec& FlagSpec::set_short_name(std::string short_name_) {
  short_name = std::move(short_name_);
  return *this;
}

namespace internal {

FlagImpl::FlagImpl(const FlagSpec& spec)
    : internal::ChoiceArgumentImpl<bool>(ChoiceArgumentSpec<bool>(spec.name)
                                             .set_short_name(spec.short_name)
                                             .set_description(spec.description)
                                             .set_help_group(spec.help_group)
                                             .set_options({{"1", true},
                                                           {"true", true},
                                                           {"TRUE", true},
                                                           {"enabled", true},
                                                           {"ENABLED", true},

                                                           {"0", false},
                                                           {"false", false},
                                                           {"FALSE", false},
                                                           {"disabled", false},
                                                           {"DISABLED", false}})
                                             .set_default_value("false")
                                             .set_implicit_value("true")) {}

bool FlagImpl::consumes_next_positional_arg() const {
  return false;
}

} // namespace internal

} // namespace mcga::cli
