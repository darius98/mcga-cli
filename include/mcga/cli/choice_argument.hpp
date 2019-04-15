#pragma once

#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include <mcga/cli/command_line_spec.hpp>

namespace mcga::cli {

template<class T>
struct ChoiceArgumentSpec {
    std::string name;
    std::string description = "";
    std::string helpGroup = "";
    std::string shortName = "";
    std::map<std::string, T> options;
    T defaultValue;
    T implicitValue;

    explicit ChoiceArgumentSpec(std::string _name): name(std::move(_name)) {}

    ChoiceArgumentSpec& setShortName(const std::string& _shortName) {
        shortName = _shortName;
        return *this;
    }

    ChoiceArgumentSpec& setDescription(const std::string& _description) {
        description = _description;
        return *this;
    }

    ChoiceArgumentSpec& setHelpGroup(const std::string& _helpGroup) {
        helpGroup = _helpGroup;
        return *this;
    }

    ChoiceArgumentSpec& setDefaultValue(const T& _defaultValue) {
        defaultValue = _defaultValue;
        return *this;
    }

    ChoiceArgumentSpec& setImplicitValue(const T& _implicitValue) {
        implicitValue = _implicitValue;
        return *this;
    }

    ChoiceArgumentSpec& setOptions(
            const std::vector<std::pair<std::string, T>>& _options) {
        options.clear();
        return addOptions(_options);
    }

    ChoiceArgumentSpec& addOptions(
            const std::vector<std::pair<std::string, T>>& _options) {
        for (const std::pair<std::string, T>& option: _options) {
            options[option.first] = option.second;
        }
        return *this;
    }

    ChoiceArgumentSpec& addOption(const std::string& key, const T& value) {
        options[key] = value;
        return *this;
    }
};

namespace internal {

template<class T>
class ChoiceArgument: public CommandLineSpec {
 public:
    T getValue() const {
        return value;
    }

    bool appeared() const {
        return appearedInArgs;
    }

 protected:
    ChoiceArgument(std::map<std::string, T> _options,
                   T _defaultValue,
                   T _implicitValue):
            options(std::move(_options)),
            defaultValue(std::move(_defaultValue)),
            implicitValue(std::move(_implicitValue)) {
        value = defaultValue;
    }

 private:
    class MakeSharedEnabler;

    void setDefault() override {
        value = defaultValue;
        appearedInArgs = false;
    }

    void setImplicit() override {
        value = implicitValue;
        appearedInArgs = true;
    }

    bool takesNextPositionalArg() const override {
        return true;
    }

    void setValue(const std::string& _value) override {
        auto optionsIterator = options.find(_value);
        if (optionsIterator == options.end()) {
            std::string renderedOptions;
            bool first = true;
            for (const std::pair<std::string, T>& option: options) {
                if (!first) {
                    renderedOptions += ",";
                }
                first = false;
                renderedOptions += "'" + option.first + "'";
            }
            throw std::invalid_argument(
                "Trying to set option `" + _value + "` to argument with "
                "options [" + renderedOptions + "]");
        }
        value = optionsIterator->second;
        appearedInArgs = true;
    }

    T value;
    std::map<std::string, T> options;
    T defaultValue;
    T implicitValue;
    bool appearedInArgs = false;

friend class mcga::cli::Parser;
};

template<class T>
class ChoiceArgument<T>::MakeSharedEnabler : public ChoiceArgument<T> {
 public:
    MakeSharedEnabler(std::map<std::string, T> options,
                      T defaultValue,
                      T implicitValue)
        : ChoiceArgument<T>(std::move(options),
                            std::move(defaultValue),
                            std::move(implicitValue)) {}
};

}  // namespace internal

template<class T>
using ChoiceArgument = std::shared_ptr<internal::ChoiceArgument<T>>;

}  // namespace mcga::cli
