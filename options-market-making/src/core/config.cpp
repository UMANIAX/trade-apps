#include "core/config.hpp"
#include "core/models/regime.hpp"
#include "core/models/regimeparams.hpp"
#include <vector>

namespace omm::core {

// Regime transition matrix: CALM -> STRESS -> EVENT (row-major order)
const double Config::REGIME_TRANSITION[3][3] = {
    {0.97, 0.03, 0.00},   // From CALM
    {0.15, 0.80, 0.05},   // From STRESS
    {0.00, 0.20, 0.80}    // From EVENT
};

std::vector<int> Config::getExpiries() {
    std::vector<int> expiries;
    for (int i = 0; i < 26; ++i) {
        expiries.push_back(4 + 7 * i);
    }
    return expiries;
}

}  // namespace omm::core
