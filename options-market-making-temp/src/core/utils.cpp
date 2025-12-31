#include "core/utils.hpp"
#include <cmath>
#include <algorithm>

namespace omm::core {

double Utils::getNormStrike(double strike, double forward, double expiry, double atmVol) {
    return std::log(strike / forward) / (atmVol * std::sqrt(expiry / 365.0));
}

double Utils::getStrikeFromNormStrike(double normStrike, double forward, double expiry, double atmVol) {
    return forward * std::exp(normStrike * atmVol * std::sqrt(expiry / 365.0));
}

std::vector<double> Utils::getNormStrikes(
    double spot,
    double interestRate,
    double expiry,
    double atmVol,
    int maxStrikeStepDist,
    double strikeStep
) {
    double forward = getForwardPrice(spot, interestRate, expiry);
    std::vector<double> normStrikes;
    
    for (int z = -maxStrikeStepDist; z <= maxStrikeStepDist; ++z) {
        double strike = spot + z * strikeStep;
        double ns = getNormStrike(strike, forward, expiry, atmVol);
        normStrikes.push_back(ns);
    }
    
    return normStrikes;
}

double Utils::getForwardPrice(double spot, double interestRate, double expiry) {
    double tte = expiry / 365.0;
    return spot * std::exp(interestRate * tte);
}

}  // namespace omm::core
