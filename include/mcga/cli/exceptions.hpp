#pragma once

#include <string>

namespace mcga::cli::internal {

[[noreturn]] void throw_logic_error(const std::string& what);

[[noreturn]] void throw_invalid_argument_exception(const std::string& what);

} // namespace mcga::cli::internal
