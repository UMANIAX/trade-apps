#include "core/workers/simulator.hpp"
#include "core/config.hpp"
#include "core/models/regimeparams.hpp"
#include <random>
#include <cmath>
#include <algorithm>

namespace omm::core::workers {

static std::mt19937 rng(std::random_device{}());
static std::normal_distribution<double> normal(0.0, 1.0);

std::map<omm::core::models::Regime, omm::core::models::RegimeParams> getRegimeParams() {
    std::map<omm::core::models::Regime, omm::core::models::RegimeParams> params;
    
    params[omm::core::models::Regime::CALM] = omm::core::models::RegimeParams(
        0.12,    // spotVol
        0.18,    // volMean
        4.0,     // volKappa
        0.20,    // volOfVol
        -0.3,    // rho
        -0.02,   // skew
        0.01     // convexity
    );
    
    params[omm::core::models::Regime::STRESS] = omm::core::models::RegimeParams(
        0.25,    // spotVol
        0.28,    // volMean
        1.5,     // volKappa
        0.45,    // volOfVol
        -0.6,    // rho
        -0.05,   // skew
        0.02     // convexity
    );
    
    params[omm::core::models::Regime::EVENT] = omm::core::models::RegimeParams(
        0.40,    // spotVol
        0.35,    // volMean
        0.5,     // volKappa
        0.80,    // volOfVol
        -0.75,   // rho
        -0.08,   // skew
        0.03     // convexity
    );
    
    return params;
}

std::shared_ptr<omm::core::models::Market> Simulator::initializeMarket() {
    omm::core::models::Asset asset(".NDX");
    
    auto expiries = Config::getExpiries();
    std::vector<double> expiriesDouble(expiries.begin(), expiries.end());
    
    for (auto& e : expiriesDouble) {
        e += Config::TIME_STEP;
    }
    
    auto volSurface = std::make_shared<omm::core::models::VolSurface>(
        expiriesDouble,
        Config::VIX,
        -0.02,  // skew
        0.01,   // convexity
        0.18,   // volMean
        Config::SPX_SPOT,
        Config::INTEREST_RATE
    );
    
    auto market = std::make_shared<omm::core::models::Market>(
        asset,
        -Config::TIME_STEP,
        Config::SPX_SPOT,
        volSurface,
        Config::INTEREST_RATE,
        omm::core::models::Regime::CALM
    );
    
    auto expiriesForSimulation = Config::getExpiries();
    std::vector<double> expiriesForSimulationDouble(expiriesForSimulation.begin(), expiriesForSimulation.end());
    for (auto& e : expiriesForSimulationDouble) {
        e += Config::TIME_STEP;
    }
    
    return simulateNextMarket(market, Config::VIX, expiriesForSimulationDouble);
}

omm::core::models::Regime Simulator::getNextRegime(omm::core::models::Regime regime) {
    const double* probs = Config::REGIME_TRANSITION[static_cast<int>(regime)];
    
    // Sample from multinomial distribution
    double r = static_cast<double>(rand()) / RAND_MAX;
    double cumProb = 0.0;
    for (int i = 0; i < 3; ++i) {
        cumProb += probs[i];
        if (r < cumProb) {
            return static_cast<omm::core::models::Regime>(i);
        }
    }
    
    return static_cast<omm::core::models::Regime>(2);
}

std::shared_ptr<omm::core::models::Market> Simulator::simulateNextMarket(
    const std::shared_ptr<omm::core::models::Market>& market,
    double atmOneMonthVol,
    const std::vector<double>& expiries
) {
    int currentTime = market->time;
    double dt = static_cast<double>(Config::TIME_STEP) / 365.0;
    
    // New expiries
    std::vector<double> newExpiries;
    if (expiries.empty()) {
        newExpiries = market->volSurface->expiries;
    } else {
        newExpiries = expiries;
    }
    
    // Remove negative expiries
    std::vector<double> filteredExpiries;
    for (double e : newExpiries) {
        if (e - Config::TIME_STEP > 0) {
            filteredExpiries.push_back(e - Config::TIME_STEP);
        }
    }
    
    int numExpiriesRemoved = newExpiries.size() - filteredExpiries.size();
    
    // Add new expiries at the end with 7-day steps
    for (int i = 0; i < numExpiriesRemoved; ++i) {
        double newExpiry = filteredExpiries.back() + 7.0 * (i + 1);
        filteredExpiries.push_back(newExpiry);
    }
    
    // Get next regime and its parameters
    auto nextRegime = getNextRegime(market->regime);
    auto regimeParamsMap = getRegimeParams();
    auto regimeParams = regimeParamsMap[nextRegime];
    
    // Generate correlated shocks
    double z1 = normal(rng);
    double z2 = normal(rng);
    double zSpot = z1;
    double zVol = regimeParams.rho * z1 + std::sqrt(1.0 - regimeParams.rho * regimeParams.rho) * z2;
    
    // Spot GBM
    double spotNext = market->spot * std::exp(
        -0.5 * regimeParams.spotVol * regimeParams.spotVol * dt +
        regimeParams.spotVol * std::sqrt(dt) * zSpot
    );
    
    // Vol Ornstein-Uhlenbeck process
    double currentAtmOneMonthVol = (atmOneMonthVol > 0) ? atmOneMonthVol : market->volSurface->atmOneMonthVolEst;
    double atmOneMonthVolNext = currentAtmOneMonthVol +
        regimeParams.volKappa * (regimeParams.volMean - currentAtmOneMonthVol) * dt +
        regimeParams.volOfVol * std::sqrt(dt) * zVol;
    
    // Create new vol surface
    auto vsNext = std::make_shared<omm::core::models::VolSurface>(
        filteredExpiries,
        atmOneMonthVolNext,
        regimeParams.skew,
        regimeParams.convexity,
        regimeParams.volMean,
        spotNext,
        market->interestRate
    );
    
    return std::make_shared<omm::core::models::Market>(
        market->asset,
        currentTime + Config::TIME_STEP,
        spotNext,
        vsNext,
        market->interestRate,
        nextRegime
    );
}

}  // namespace omm::core::workers
