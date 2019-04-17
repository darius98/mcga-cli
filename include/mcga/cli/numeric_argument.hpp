#pragma once

#include <string>
#include <type_traits>

#include "command_line_spec.hpp"
#include "disallow_copy_and_move.hpp"

namespace mcga::cli {

template<class T,
         class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
struct NumericArgumentSpec {
    std::string name;
    std::string description = "";
    std::string helpGroup = "";
    std::string shortName = "";
    bool hasDefaultValue = false;
    T defaultValue;
    bool hasImplicitValue = false;
    T implicitValue;

    explicit NumericArgumentSpec(std::string _name): name(std::move(_name)) {
    }

    NumericArgumentSpec& setDescription(std::string _description) {
        description = std::move(_description);
        return *this;
    }

    NumericArgumentSpec& setHelpGroup(std::string _helpGroup) {
        helpGroup = std::move(_helpGroup);
        return *this;
    }

    NumericArgumentSpec& setShortName(std::string _shortName) {
        shortName = std::move(_shortName);
        return *this;
    }

    NumericArgumentSpec& setDefaultValue(const T& _defaultValue) {
        defaultValue = _defaultValue;
        hasDefaultValue = true;
        return *this;
    }

    NumericArgumentSpec& setImplicitValue(const T& _implicitValue) {
        implicitValue = _implicitValue;
        hasImplicitValue = true;
        return *this;
    }
};

namespace internal {

template<class T,
         class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
class NumericArgument : public CommandLineSpec {
  public:
    ~NumericArgument() override = default;

    T getValue() const {
        return value;
    }

    bool appeared() const override {
        return appearedInArgs;
    }

  private:
    class MakeSharedEnabler;

    explicit NumericArgument(const NumericArgumentSpec<T>& spec): spec(spec) {
    }

    MCGA_DISALLOW_COPY_AND_MOVE(NumericArgument);

    void setDefault() override {
        if (!spec.hasDefaultValue) {
            throw std::invalid_argument(
              "Trying to set default value for argument " + spec.name
              + ", which has no default value.");
        }
        value = spec.defaultValue;
        appearedInArgs = false;
    }

    void setImplicit() override {
        if (!spec.hasImplicitValue) {
            throw std::invalid_argument(
              "Trying to set implicit value for argument " + spec.name
              + ", which has no implicit value.");
        }
        value = spec.implicitValue;
        appearedInArgs = true;
    }

    bool takesNextPositionalArg() const override {
        return true;
    }

    void setValue(const std::string& _value) override;

    NumericArgumentSpec<T> spec;
    T value;
    bool appearedInArgs = false;

    friend class mcga::cli::Parser;
};

template<class T, class S>
class NumericArgument<T, S>::MakeSharedEnabler : public NumericArgument<T, S> {
  public:
    explicit MakeSharedEnabler(const NumericArgumentSpec<T>& spec)
            : NumericArgument<T, S>(spec) {
    }
};

}  // namespace internal

template<class T>
using NumericArgument = std::shared_ptr<internal::NumericArgument<T>>;

namespace internal {

template<>
inline void NumericArgument<char>::setValue(const std::string& _value) {
    value = static_cast<char>(std::stoi(_value));
    appearedInArgs = true;
}

template<>
inline void
  NumericArgument<unsigned char>::setValue(const std::string& _value) {
    value = static_cast<unsigned char>(std::stoul(_value));
    appearedInArgs = true;
}

template<>
inline void NumericArgument<short int>::setValue(const std::string& _value) {
    value = static_cast<short int>(std::stoi(_value));
    appearedInArgs = true;
}

template<>
inline void
  NumericArgument<unsigned short int>::setValue(const std::string& _value) {
    value = static_cast<unsigned short int>(std::stoul(_value));
    appearedInArgs = true;
}

template<>
inline void NumericArgument<int>::setValue(const std::string& _value) {
    value = std::stoi(_value);
    appearedInArgs = true;
}

template<>
inline void NumericArgument<unsigned int>::setValue(const std::string& _value) {
    value = static_cast<unsigned int>(std::stoul(_value));
    appearedInArgs = true;
}

template<>
inline void NumericArgument<long>::setValue(const std::string& _value) {
    value = std::stol(_value);
    appearedInArgs = true;
}

template<>
inline void
  NumericArgument<unsigned long>::setValue(const std::string& _value) {
    value = std::stoul(_value);
    appearedInArgs = true;
}

template<>
inline void NumericArgument<long long>::setValue(const std::string& _value) {
    value = std::stoll(_value);
    appearedInArgs = true;
}

template<>
inline void
  NumericArgument<unsigned long long>::setValue(const std::string& _value) {
    value = std::stoull(_value);
    appearedInArgs = true;
}

template<>
inline void NumericArgument<float>::setValue(const std::string& _value) {
    value = std::stof(_value);
    appearedInArgs = true;
}

template<>
inline void NumericArgument<double>::setValue(const std::string& _value) {
    value = std::stod(_value);
    appearedInArgs = true;
}

template<>
inline void NumericArgument<long double>::setValue(const std::string& _value) {
    value = std::stold(_value);
    appearedInArgs = true;
}

}  // namespace internal

}  // namespace mcga::cli
