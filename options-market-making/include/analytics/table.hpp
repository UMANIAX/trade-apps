#pragma once

#include "core/models/market.hpp"
#include "core/models/option.hpp"
#include <map>
#include <string>
#include <vector>

namespace omm::analytics {

struct OptionChainRow {
    double callTheta;
    double callVega;
    double callGamma;
    double callDelta;
    double callPrice;
    double normStrike;
    double strike;
    double forwardStrike;
    double impliedVol;
    double putPrice;
    double putDelta;
    double putGamma;
    double putVega;
    double putTheta;
};

class Table {
public:
    // Generate option chain table for a given expiry
    static std::vector<OptionChainRow> getOptionChainTable(
        const std::shared_ptr<omm::core::models::Market>& market,
        double expiry
    );
    
    // Print option chain table to console
    static void printOptionChainTable(const std::vector<OptionChainRow>& chain);
};

}  // namespace omm::analytics
