#pragma once

#include <vector>
#include <cmath>

namespace omm::core {

class Utils {
public:
    // Calculate normalized strike
    static double getNormStrike(double strike, double forward, double expiry, double atmVol);
    
    // Calculate strike from normalized strike
    static double getStrikeFromNormStrike(double normStrike, double forward, double expiry, double atmVol);
    
    // Get array of normalized strikes
    static std::vector<double> getNormStrikes(
        double spot, 
        double interestRate, 
        double expiry, 
        double atmVol,
        int maxStrikeStepDist = 40, 
        double strikeStep = 50.0
    );
    
    // Calculate forward price using risk-free rate
    static double getForwardPrice(double spot, double interestRate, double expiry);
};

}  // namespace omm::core
