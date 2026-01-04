#pragma once

#include <unordered_map>
#include <vector>
#include "Regime.h"

struct SimulatorConfig {
    double TIME_STEP = 1;
    std::vector<std::vector<double>> REGIME_TRANSITION_MATRIX = {
        {0.90, 0.08, 0.02},
        {0.10, 0.85, 0.05},
        {0.20, 0.20, 0.60}
    };
    std::unordered_map<RegimeType, RegimeParams> REGIME_PARAM_MAP = {
        {RegimeType::CALM,   RegimeParams(0.05, 0.15, 0.10, 1.0, 0.02)},
        {RegimeType::STRESS, RegimeParams(0.02, 0.35, 0.30, 2.0, 0.08)},
        {RegimeType::EVENT,  RegimeParams(0.00, 0.60, 0.50, 0.5, 0.15)}
    };
};