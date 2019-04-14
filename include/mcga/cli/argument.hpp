#pragma once

#include <string>

#include <mcga/cli/command_line_spec.hpp>
#include <mcga/cli/disallow_copy_and_move.hpp>

namespace mcga::cli {

class Parser;

struct ArgumentSpec {
    std::string name;
    std::string description = "";
    std::string helpGroup = "";
    std::string shortName = "";
    std::string defaultValue = "";
    std::string implicitValue = "";

    explicit ArgumentSpec(std::string name): name(std::move(name)) {}

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

class Argument: public CommandLineSpec {
 public:
    MCGA_DISALLOW_COPY_AND_MOVE(Argument);

    ~Argument() override = default;

    std::string get() const {
        return value;
    }

    bool appeared() const {
        return appearedInArgs;
    }

 private:
    class MakeSharedEnabler;

    Argument(std::string defaultValue, std::string implicitValue)
            : defaultValue(std::move(defaultValue)),
              implicitValue(std::move(implicitValue)) {}

    bool takesNextPositionalArg() const override {
        return true;
    }

    void setDefault() override {
        value = defaultValue;
        appearedInArgs = false;
    }

    void setImplicit() override {
        value = implicitValue;
        appearedInArgs = true;
    }

    void setValue(const std::string& _value) override {
        value = _value;
        appearedInArgs = true;
    }

 private:
    std::string value;
    std::string defaultValue;
    std::string implicitValue;
    bool appearedInArgs = false;

friend class mcga::cli::Parser;
};

class Argument::MakeSharedEnabler: public Argument {
 public:
    MakeSharedEnabler(std::string defaultValue, std::string implicitValue)
            : Argument(std::move(defaultValue), std::move(implicitValue)) {}
};

}  // namespace internal

using Argument = std::shared_ptr<internal::Argument>;

}  // namespace mcga::cli
