#pragma once
#pragma ide diagnostic ignored "google-runtime-int"

#include <string>
#include <type_traits>

#include <mcga/cli/command_line_spec.hpp>

namespace mcga::cli {

template<class T,
         class=typename std::enable_if<std::is_arithmetic<T>::value>::type>
struct NumericArgumentSpec {
    std::string name;
    std::string description = "";
    std::string helpGroup = "";
    std::string shortName = "";
    T defaultValue = 0;
    T implicitValue = 0;

    explicit NumericArgumentSpec(std::string _name): name(std::move(_name)) {}

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

    NumericArgumentSpec& setDefaultValue(T _defaultValue) {
        defaultValue = std::move(_defaultValue);
        return *this;
    }

    NumericArgumentSpec& setImplicitValue(T _implicitValue) {
        implicitValue = std::move(_implicitValue);
        return *this;
    }
};

namespace internal {

template<class T,
         class=typename std::enable_if<std::is_arithmetic<T>::value>::type>
class NumericArgument: public CommandLineSpec {
 public:
    T getValue() const {
        return value;
    }

    bool appeared() const {
        return appearedInArgs;
    }

 private:
    class MakeSharedEnabler;

    NumericArgument(T _defaultValue, T _implicitValue):
            defaultValue(_defaultValue), implicitValue(_implicitValue) {}

    ~NumericArgument() override = default;

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

    void setValue(const std::string& _value) override;

    T value;
    T defaultValue;
    T implicitValue;
    bool appearedInArgs = false;

friend class mcga::cli::Parser;
};

template<class T, class S>
class NumericArgument<T, S>::MakeSharedEnabler : public NumericArgument<T, S> {
 public:
    MakeSharedEnabler(T _defaultValue, T _implicitValue)
            : NumericArgument<T, S>(std::move(_defaultValue),
                                    std::move(_implicitValue)) {}
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
inline void NumericArgument<unsigned char>::setValue(
        const std::string& _value) {
    value = static_cast<unsigned char>(std::stoul(_value));
    appearedInArgs = true;
}

template<>
inline void NumericArgument<short int>::setValue(const std::string& _value) {
    value = static_cast<short int>(std::stoi(_value));
    appearedInArgs = true;
}

template<>
inline void NumericArgument<unsigned short int>::setValue(
        const std::string& _value) {
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
inline void NumericArgument<unsigned long>::setValue(
        const std::string& _value) {
    value = std::stoul(_value);
    appearedInArgs = true;
}

template<>
inline void NumericArgument<long long>::setValue(const std::string& _value) {
    value = std::stoll(_value);
    appearedInArgs = true;
}

template<>
inline void NumericArgument<unsigned long long>::setValue(
        const std::string& _value) {
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
