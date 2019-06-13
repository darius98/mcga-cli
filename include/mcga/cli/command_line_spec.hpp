#pragma once

#include <string>

#include "disallow_copy_and_move.hpp"

namespace mcga::cli {

class Parser;

}  // namespace mcga::cli

namespace mcga::cli::internal {

class CommandLineSpec {
  public:
    bool appeared() const {
        return appearedInArgs;
    }

  protected:
    CommandLineSpec(bool _hasDefaultValue, bool _hasImplicitValue)
            : hasDefaultValue(_hasDefaultValue),
              hasImplicitValue(_hasImplicitValue) {
    }

    ~CommandLineSpec() = default;

  private:
    MCGA_DISALLOW_COPY_AND_MOVE(CommandLineSpec);

    virtual const std::string& getName() const = 0;

    virtual bool takesNextPositionalArg() const {
        return true;
    }

    virtual void setDefault() = 0;

    virtual void setImplicit() = 0;

    virtual void setValue(const std::string& value) = 0;

    void setDefaultGuarded() {
        if (!hasDefaultValue) {
            throw std::invalid_argument(
              "Trying to set default value for argument " + getName()
              + ", which has no default value.");
        }
        setDefault();
        appearedInArgs = false;
    }

    void setImplicitGuarded() {
        if (!hasImplicitValue) {
            throw std::invalid_argument(
              "Trying to set implicit value for argument " + getName()
              + ", which has no implicit value.");
        }
        setImplicit();
        appearedInArgs = true;
    }

    void setValueGuarded(const std::string& value) {
        setValue(value);
        appearedInArgs = true;
    }

    bool appearedInArgs = false;
    bool hasDefaultValue;
    bool hasImplicitValue;

    friend class mcga::cli::Parser;
};

}  // namespace mcga::cli::internal
