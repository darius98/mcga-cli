#pragma once

#include <string>

#include "disallow_copy_and_move.hpp"

namespace mcga::cli {

class Parser;

} // namespace mcga::cli

namespace mcga::cli::internal {

class CommandLineOption {
public:
  [[nodiscard]] bool appeared() const;

protected:
  CommandLineOption(bool has_default_value_, bool has_implicit_value_);

  virtual ~CommandLineOption() = default;

  virtual void reset();

private:
  MCGA_DISALLOW_COPY_AND_MOVE(CommandLineOption);

  [[nodiscard]] virtual const std::string& get_name() const = 0;

  [[nodiscard]] virtual bool consumes_next_positional_arg() const;

  virtual void set_default() = 0;

  virtual void set_implicit() = 0;

  virtual void set_value(const std::string& value) = 0;

  void set_default_guarded();

  void set_implicit_guarded();

  void set_value_guarded(const std::string& value);

  bool appeared_in_args = false;
  bool has_default_value;
  bool has_implicit_value;

  friend class mcga::cli::Parser;
};

} // namespace mcga::cli::internal
