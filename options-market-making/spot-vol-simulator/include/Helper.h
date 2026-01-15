

#pragma once
#include <vector>

inline double getForwardPrice(double spot, double riskFreeRate, double expiry) {
    double tte = expiry / 365.0;
    return spot * std::exp(riskFreeRate * tte);
}

inline double getNormStrikeFromStrike(double strike, double forward, double expiry, double atmVol) {
    return std::log(strike / forward) / (atmVol * std::sqrt(expiry / 365.0));
}

inline std::vector<double> getNormStrikesForExpiry(double spot, double riskFreeRate, double expiry, double atmVol, double maxStrikeDepth, double strikeStep) {
    double forward = getForwardPrice(spot, riskFreeRate, expiry);
    std::vector<double> normStrikes;
    for (int idx = -maxStrikeDepth; idx <= maxStrikeDepth; idx += strikeStep) {
        double strike  = spot + idx;
        double ns = getNormStrikeFromStrike(strike, forward, expiry, atmVol);
        normStrikes.push_back(ns);
    }
    return normStrikes;
}