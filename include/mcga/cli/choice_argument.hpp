#pragma once

#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "command_line_spec.hpp"
#include "disallow_copy_and_move.hpp"

namespace mcga::cli {

template<class T>
struct ChoiceArgumentSpec {
    std::string name;
    std::string description = "";
    std::string helpGroup = "";
    std::string shortName = "";
    std::map<std::string, T> options;
    bool hasDefaultValue = false;
    std::string defaultValue;
    bool hasImplicitValue = false;
    std::string implicitValue;

    explicit ChoiceArgumentSpec(std::string _name): name(std::move(_name)) {
    }

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

    ChoiceArgumentSpec& setDefaultValue(const std::string& _defaultValue) {
        defaultValue = _defaultValue;
        hasDefaultValue = true;
        return *this;
    }

    ChoiceArgumentSpec& setImplicitValue(const std::string& _implicitValue) {
        implicitValue = _implicitValue;
        hasImplicitValue = true;
        return *this;
    }

    ChoiceArgumentSpec&
      setOptions(const std::vector<std::pair<std::string, T>>& _options) {
        options.clear();
        return addOptions(_options);
    }

    ChoiceArgumentSpec&
      addOptions(const std::vector<std::pair<std::string, T>>& _options) {
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
class ChoiceArgument : public CommandLineSpec {
  public:
    ~ChoiceArgument() override = default;

    const ChoiceArgumentSpec<T>& getSpec() const {
        return spec;
    }

    T getValue() const {
        return value;
    }

  protected:
    explicit ChoiceArgument(const ChoiceArgumentSpec<T>& spec)
            : CommandLineSpec(spec.hasDefaultValue, spec.hasImplicitValue),
              spec(spec) {
    }

  private:
    MCGA_DISALLOW_COPY_AND_MOVE(ChoiceArgument);

    class MakeSharedEnabler;

    const std::string& getName() const override {
        return spec.name;
    }

    void setDefault() override {
        setValue(spec.defaultValue);
    }

    void setImplicit() override {
        setValue(spec.implicitValue);
    }

    void setValue(const std::string& _value) override {
        auto optionsIterator = spec.options.find(_value);
        if (optionsIterator == spec.options.end()) {
            std::string renderedOptions;
            bool first = true;
            for (const std::pair<std::string, T>& option: spec.options) {
                if (!first) {
                    renderedOptions += ",";
                }
                first = false;
                renderedOptions += "'" + option.first + "'";
            }
            throw std::invalid_argument(
              "Trying to set option `" + _value + "` to argument " + spec.name
              + ", which has options [" + renderedOptions + "]");
        }
        value = optionsIterator->second;
    }

    ChoiceArgumentSpec<T> spec;
    T value;

    friend class mcga::cli::Parser;
};

template<class T>
class ChoiceArgument<T>::MakeSharedEnabler : public ChoiceArgument<T> {
  public:
    explicit MakeSharedEnabler(const ChoiceArgumentSpec<T>& spec)
            : ChoiceArgument<T>(spec) {
    }
};

}  // namespace internal

template<class T>
using ChoiceArgument = std::shared_ptr<internal::ChoiceArgument<T>>;

}  // namespace mcga::cli
