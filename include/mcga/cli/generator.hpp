#pragma once

#include <functional>
#include <string>
#include <vector>

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

class ListGenerator {
public:
  ListGenerator(std::function<std::vector<std::string>()> generator_,
                std::string description_);

  [[nodiscard]] std::vector<std::string> generate() const;

  [[nodiscard]] const std::string& get_description() const;

private:
  std::function<std::vector<std::string>()> generator;
  std::string description;
};

} // namespace mcga::cli::internal
