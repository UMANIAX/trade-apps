#pragma once

#include <string>
#include <memory>

namespace omm::core::models {

class Asset {
public:
    std::string symbol;
    
    Asset() = default;
    explicit Asset(const std::string& symbol_) : symbol(symbol_) {}
};

}  // namespace omm::core::models
