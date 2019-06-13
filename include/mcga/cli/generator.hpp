#pragma once

#include <functional>
#include <string>

namespace mcga::cli::internal {

class Generator {
  private:
    std::function<std::string()> generator;
    std::string description;

  public:
    Generator(std::function<std::string()> _generator, std::string _description)
            : generator(std::move(_generator)),
              description(std::move(_description)) {
    }

    std::string generate() const {
        return generator();
    }

    const std::string& getDescription() const {
        return description;
    }
};

}  // namespace mcga::cli::internal
