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
    std::string defaultValue = "";
    std::string implicitValue = "";

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
        return *this;
    }

    ArgumentSpec& setImplicitValue(const std::string& _implicitValue) {
        implicitValue = _implicitValue;
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

    bool appeared() const {
        return appearedInArgs;
    }

    const ArgumentSpec& getSpec() const {
        return spec;
    }

  private:
    class MakeSharedEnabler;

    explicit Argument(ArgumentSpec spec): spec(std::move(spec)) {
    }

    MCGA_DISALLOW_COPY_AND_MOVE(Argument);

    bool takesNextPositionalArg() const override {
        return true;
    }

    void setDefault() override {
        value = spec.defaultValue;
        appearedInArgs = false;
    }

    void setImplicit() override {
        value = spec.implicitValue;
        appearedInArgs = true;
    }

    void setValue(const std::string& _value) override {
        value = _value;
        appearedInArgs = true;
    }

    ArgumentSpec spec;
    std::string value;
    bool appearedInArgs = false;

    friend class mcga::cli::Parser;
};

class Argument::MakeSharedEnabler : public Argument {
  public:
    explicit MakeSharedEnabler(ArgumentSpec spec): Argument(std::move(spec)) {
    }
};

}  // namespace internal

using Argument = std::shared_ptr<internal::Argument>;

}  // namespace mcga::cli
