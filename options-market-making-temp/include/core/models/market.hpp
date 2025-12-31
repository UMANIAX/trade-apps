#pragma once

#include "asset.hpp"
#include "volsurface.hpp"
#include "regime.hpp"

namespace omm::core::models {

class Market {
public:
    Asset asset;
    int time;
    double spot;
    std::shared_ptr<VolSurface> volSurface;
    double interestRate;
    Regime regime;
    
    Market() = default;
    Market(const Asset& asset_, int time_, double spot_,
           std::shared_ptr<VolSurface> volSurface_, double interestRate_,
           Regime regime_)
        : asset(asset_), time(time_), spot(spot_), volSurface(volSurface_),
          interestRate(interestRate_), regime(regime_) {}
};

}  // namespace omm::core::models
