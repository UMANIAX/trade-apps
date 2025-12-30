#pragma once

#include "core/models/market.hpp"
#include <memory>
#include <vector>

namespace omm::core::workers {

class Simulator {
public:
    // Initialize market with initial conditions
    static std::shared_ptr<omm::core::models::Market> initializeMarket();
    
    // Get next regime based on transition probabilities
    static omm::core::models::Regime getNextRegime(omm::core::models::Regime regime);
    
    // Simulate the next market state
    static std::shared_ptr<omm::core::models::Market> simulateNextMarket(
        const std::shared_ptr<omm::core::models::Market>& market,
        double atmOneMonthVol = -1.0,
        const std::vector<double>& expiries = {}
    );
};

}  // namespace omm::core::workers
