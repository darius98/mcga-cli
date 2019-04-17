#pragma once

#include <string>

#include "disallow_copy_and_move.hpp"

namespace mcga::cli {

class Parser;

}  // namespace mcga::cli

namespace mcga::cli::internal {

class CommandLineSpec {
  protected:
    CommandLineSpec() = default;

    virtual ~CommandLineSpec() = default;

  private:
    MCGA_DISALLOW_COPY_AND_MOVE(CommandLineSpec);

    virtual bool appeared() const = 0;

    virtual bool takesNextPositionalArg() const = 0;

    virtual void setDefault() = 0;

    virtual void setImplicit() = 0;

    virtual void setValue(const std::string& value) = 0;

    friend class mcga::cli::Parser;
};

}  // namespace mcga::cli::internal
