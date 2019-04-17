#pragma once

#include <string>

#include "command_line_spec.hpp"
#include "disallow_copy_and_move.hpp"

namespace mcga::cli {

class Parser;

struct ArgumentSpec {
    std::string name;
    std::string description = "";
    std::string helpGroup = "";
    std::string shortName = "";
    bool hasDefaultValue = false;
    std::string defaultValue;
    bool hasImplicitValue = false;
    std::string implicitValue;

    explicit ArgumentSpec(std::string name): name(std::move(name)) {
    }

    ArgumentSpec& setDescription(std::string _description) {
        description = std::move(_description);
        return *this;
    }

    ArgumentSpec& setHelpGroup(std::string _helpGroup) {
        helpGroup = std::move(_helpGroup);
        return *this;
    }

    ArgumentSpec& setShortName(std::string _shortName) {
        shortName = std::move(_shortName);
        return *this;
    }

    ArgumentSpec& setDefaultValue(const std::string& _defaultValue) {
        defaultValue = _defaultValue;
        hasDefaultValue = true;
        return *this;
    }

    ArgumentSpec& setImplicitValue(const std::string& _implicitValue) {
        implicitValue = _implicitValue;
        hasImplicitValue = true;
        return *this;
    }
};

namespace internal {

class Argument : public CommandLineSpec {
  public:
    ~Argument() override = default;

    std::string getValue() const {
        return value;
    }

    const ArgumentSpec& getSpec() const {
        return spec;
    }

  private:
    class MakeSharedEnabler;

    explicit Argument(const ArgumentSpec& spec)
            : CommandLineSpec(spec.hasDefaultValue, spec.hasImplicitValue),
              spec(spec) {
    }

    MCGA_DISALLOW_COPY_AND_MOVE(Argument);

    const std::string& getName() const override {
        return spec.name;
    }

    void setDefault() override {
        value = spec.defaultValue;
    }

    void setImplicit() override {
        value = spec.implicitValue;
    }

    void setValue(const std::string& _value) override {
        value = _value;
    }

    ArgumentSpec spec;
    std::string value;

    friend class mcga::cli::Parser;
};

class Argument::MakeSharedEnabler : public Argument {
  public:
    explicit MakeSharedEnabler(const ArgumentSpec& spec): Argument(spec) {
    }
};

}  // namespace internal

using Argument = std::shared_ptr<internal::Argument>;

}  // namespace mcga::cli
