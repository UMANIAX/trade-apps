#pragma once

namespace omm::core::models {

struct RegimeParams {
    double spotVol;      // Spot volatility
    double volMean;      // Mean reversion level for vol
    double volKappa;     // Mean reversion speed
    double volOfVol;     // Volatility of volatility
    double rho;          // Spot-vol correlation
    double skew;         // Volatility skew
    double convexity;    // Volatility convexity
    
    RegimeParams(
        double spotVol_ = 0.0,
        double volMean_ = 0.0,
        double volKappa_ = 0.0,
        double volOfVol_ = 0.0,
        double rho_ = 0.0,
        double skew_ = 0.0,
        double convexity_ = 0.0
    ) : spotVol(spotVol_), volMean(volMean_), volKappa(volKappa_),
        volOfVol(volOfVol_), rho(rho_), skew(skew_), convexity(convexity_) {}
};

}  // namespace omm::core::models
