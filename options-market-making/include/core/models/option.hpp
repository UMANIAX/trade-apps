#pragma once

#include "security.hpp"
#include "asset.hpp"
#include "optiontype.hpp"

namespace omm::core::models {

class Option : public Security {
public:
    Asset asset;
    double strike;
    double expiry;
    OptionType optionType;
    int lotSize;
    
    Option() = default;
    Option(const Asset& asset_, double strike_, double expiry_, 
           OptionType optionType_, int lotSize_)
        : asset(asset_), strike(strike_), expiry(expiry_), 
          optionType(optionType_), lotSize(lotSize_) {}
};

}  // namespace omm::core::models
