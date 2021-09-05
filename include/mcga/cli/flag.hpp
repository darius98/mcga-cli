#pragma once

#include <memory>
#include <string>

#include "choice_argument.hpp"
#include "command_line_option.hpp"

namespace mcga::cli {

struct FlagSpec {
  std::string name;
  std::string description = "";
  std::string help_group = "";
  std::string short_name = "";

  explicit FlagSpec(std::string name_);

  FlagSpec& set_description(std::string description_);
  FlagSpec& set_help_group(std::string help_group_);
  FlagSpec& set_short_name(std::string short_name_);
};

namespace internal {

class FlagImpl: public internal::ChoiceArgumentImpl<bool> {
public:
  explicit FlagImpl(const FlagSpec& spec);

  ~FlagImpl() override = default;

private:
  MCGA_DISALLOW_COPY_AND_MOVE(FlagImpl);

  [[nodiscard]] bool consumes_next_positional_arg() const override;

  friend class mcga::cli::Parser;
};

} // namespace internal

struct Flag : std::shared_ptr<internal::FlagImpl> {
    using ValueType = bool;
    using std::shared_ptr<internal::FlagImpl>::shared_ptr;
};

} // namespace mcga::cli
