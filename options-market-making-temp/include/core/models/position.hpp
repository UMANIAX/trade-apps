#pragma once

#include "security.hpp"
#include <memory>

namespace omm::core::models {

class Position {
public:
    std::shared_ptr<Security> security;
    int quantity;  // Positive for long, negative for short
    
    Position() = default;
    Position(std::shared_ptr<Security> security_, int quantity_)
        : security(security_), quantity(quantity_) {}
};

}  // namespace omm::core::models
