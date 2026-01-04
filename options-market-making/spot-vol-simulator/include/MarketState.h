#pragma once

#include "VolSurface.h"
#include "Regime.h"

struct MarketState {
    double time;
    double spot;
    VolSurface volSurface;
    Regime regime;
    double riskFreeRate;

    MarketState(
        double time_,
        double spot_,
        VolSurface& volSurface_,
        Regime& regime_,
        double riskFreeRate_
    ) :
        time(time_),
        spot(spot_),
        volSurface(volSurface_),
        regime(regime_),
        riskFreeRate(riskFreeRate_) {}
};