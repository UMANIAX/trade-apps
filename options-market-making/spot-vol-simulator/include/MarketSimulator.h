#pragma once

#include <random>
#include "SimulatorConfig.h"
#include "MarketState.h"
#include "Regime.h"

class MarketSimulator {
public:
    MarketSimulator(const SimulatorConfig& config) :
        config_(config),
        rng_(42) {}

    MarketState evolve(const MarketState& current);
private:
    SimulatorConfig config_;
    std::mt19937 rng_;

    double setSpotShock();
    Regime evolveRegime(const Regime& regime);
    double evolveSpot(double spot, const Regime& regime, double spotShock);
    VolSurface evolveVol(const VolSurface& volSurface, const Regime& regime, double spotShock);
    MarketState evolveMkt();
};