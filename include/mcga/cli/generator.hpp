#pragma once

#include <functional>
#include <string>

namespace mcga::cli::internal {

class Generator {
public:
  Generator(std::function<std::string()> generator_, std::string description_);

  [[nodiscard]] std::string generate() const;

  [[nodiscard]] const std::string& get_description() const;

private:
  std::function<std::string()> generator;
  std::string description;
};

} // namespace mcga::cli::internal
