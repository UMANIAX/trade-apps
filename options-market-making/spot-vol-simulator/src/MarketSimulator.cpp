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

double MarketSimulator::evolveSpot(double current, const Regime& regime, double spotShock) {
    double dt = config_.TIME_STEP / 365.0;
    double drift = regime.params.spotDrift;
    double spotVol = regime.params.spotVol;
    // spot GBM: dS(t) = μ*S(t)*dt + σ*S(t)*dW --Ito-Calculus--> s(t+1) = s(t) * e^(μ*dt - σ^2*dt/2 + σ*dW) 
    return current * std::exp(drift * dt - std::pow(spotVol, 2) * dt / 2 + spotVol * std::sqrt(dt) * spotShock);
}

VolSurface MarketSimulator::evolveVol(const VolSurface& current, const Regime& regime, double spotShock) {
    std::normal_distribution<double> normal(0.0, 1.0);

    double dt = config_.TIME_STEP / 365.0;
    double volOfVol = regime.params.volOfVol;
    double volKappa = regime.params.volKappa;
    double volMean = regime.params.volMean;
    double rho = regime.params.rho;
    double skew = regime.params.skew;
    double convexity = regime.params.convexity;
    double volShock = rho * spotShock + std::sqrt(1.0 - std::pow(rho, 2)) * normal(rng_);
    double curr_vol_est = current.getVol1mEst();
    double next_atm_est = curr_vol_est + volKappa * (volMean - curr_vol_est) * dt + volOfVol * std::sqrt(dt) * volShock;

    // COMPLETE
}