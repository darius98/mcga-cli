#include <mcga/cli/generator.hpp>

namespace mcga::cli::internal {

Generator::Generator(std::function<std::string()> generator_,
                     std::string description_)
    : generator(std::move(generator_)), description(std::move(description_)) {}

std::string Generator::generate() const {
  return generator();
}

const std::string& Generator::get_description() const {
  return description;
}

} // namespace mcga::cli::internal
