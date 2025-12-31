#pragma once

#include "security.hpp"
#include "asset.hpp"

namespace omm::core::models {

class Future : public Security {
public:
    Asset asset;
    std::string expiry;
    int lotSize;
    
    Future() = default;
    Future(const Asset& asset_, const std::string& expiry_, int lotSize_)
        : asset(asset_), expiry(expiry_), lotSize(lotSize_) {}
};

}  // namespace omm::core::models
