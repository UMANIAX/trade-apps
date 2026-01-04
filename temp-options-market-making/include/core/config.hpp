#pragma once

#include <string>
#include <map>
#include <vector>

namespace omm::core {

// Configuration constants
class Config {
public:
    static constexpr int TIME_STEP = 1;  // 1 = 1 day
    static constexpr double SPX_SPOT = 25000.0;
    static constexpr double VIX = 0.22;
    static constexpr double INTEREST_RATE = 0.05;
    
    static std::vector<int> getExpiries();
    
    // Regime transition matrix [3x3] in row-major order
    static const double REGIME_TRANSITION[3][3];
};

}  // namespace omm::core
