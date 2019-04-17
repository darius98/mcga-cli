#pragma once

#include <string>

#include "choice_argument.hpp"
#include "command_line_spec.hpp"

namespace mcga::cli {

struct FlagSpec {
    std::string name;
    std::string description = "";
    std::string helpGroup = "";
    std::string shortName = "";

    explicit FlagSpec(std::string _name): name(std::move(_name)) {
    }

    FlagSpec& setDescription(std::string _description) {
        description = std::move(_description);
        return *this;
    }

    FlagSpec& setHelpGroup(std::string _helpGroup) {
        helpGroup = std::move(_helpGroup);
        return *this;
    }

    FlagSpec& setShortName(std::string _shortName) {
        shortName = std::move(_shortName);
        return *this;
    }
};

namespace internal {

class Flag : public internal::ChoiceArgument<bool> {
  public:
    ~Flag() override = default;

  private:
    class FlagMakeSharedEnabler;

    explicit Flag(const FlagSpec& spec)
            : internal::ChoiceArgument<bool>(
              ChoiceArgumentSpec<bool>(spec.name)
                .setShortName(spec.shortName)
                .setDescription(spec.description)
                .setHelpGroup(spec.helpGroup)
                .setOptions({{"1", true},
                             {"true", true},
                             {"TRUE", true},
                             {"enabled", true},
                             {"ENABLED", true},

                             {"0", false},
                             {"false", false},
                             {"FALSE", false},
                             {"disabled", false},
                             {"DISABLED", false}})
                .setDefaultValue("false")
                .setImplicitValue("true")) {
    }

    MCGA_DISALLOW_COPY_AND_MOVE(Flag);

    bool takesNextPositionalArg() const override {
        return false;
    }

    friend class mcga::cli::Parser;
};

class Flag::FlagMakeSharedEnabler : public Flag {
  public:
    explicit FlagMakeSharedEnabler(const FlagSpec& spec): Flag(spec) {
    }
};

}  // namespace internal

using Flag = std::shared_ptr<internal::Flag>;

}  // namespace mcga::cli
