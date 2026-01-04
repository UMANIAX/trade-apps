#include "MarketSimulator.h"
#include <cmath>
#include <stdexcept>

Regime MarketSimulator::evolveRegime(const Regime& current) {
    std::uniform_real_distribution<double> unif(0.0, 1.0);
    double cumulative_prob = unif(rng_);
    double current_prob = 0.0;
    int currentIdx = static_cast<int>(current.type);
    for (int nextIdx = 0; nextIdx < static_cast<int>(RegimeType::COUNT); nextIdx++) {
        current_prob += config_.REGIME_TRANSITION_MATRIX[currentIdx][nextIdx];
        if (current_prob >= cumulative_prob) {
            RegimeType nextType = static_cast<RegimeType>(nextIdx);
            RegimeParams nextParams = config_.REGIME_PARAM_MAP[nextType];
            return Regime(nextType, nextParams);
        }
    }
    throw std::runtime_error("Regime Transition Failed!");
}

double MarketSimulator::setSpotShock() {
    std::normal_distribution<double> normal(0.0, 1.0);
    spotShock_ = normal(rng_);
}

double MarketSimulator::evolveSpot(double current, const Regime& regime) {
    double dt = config_.TIME_STEP / 365;
    double drift = regime.params.spotDrift;
    double vol = regime.params.spotVol;
    // spot GBM: dS(t) = μ*dt + σ*S(t)*dW --Ito-Calculus--> s(t+1) = s(t) * e^(μ*dt - σ^2*dt/2 + σ*dW) 
    return current * std::exp(drift * dt - std::pow(vol, 2) * dt / 2 + vol * std::sqrt(dt) * spotShock_);
}