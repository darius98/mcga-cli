#include <mcga/cli/exceptions.hpp>

#ifdef __EXCEPTIONS
#include <stdexcept>
#endif

namespace mcga::cli::internal {

void throw_logic_error(const std::string& what) {
#ifdef __EXCEPTIONS
  throw std::logic_error(what);
#else
  std::cerr << what << "\n";
  std::abort();
#endif
}

void throw_invalid_argument_exception(const std::string& what) {
#ifdef __EXCEPTIONS
  throw std::invalid_argument(what);
#else
  std::cerr << what << "\n";
  std::abort();
#endif
}

} // namespace mcga::cli::internal
