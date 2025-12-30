#include "analytics/table.hpp"
#include "core/utils.hpp"
#include "core/workers/calculator.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

namespace omm::analytics {

std::vector<OptionChainRow> Table::getOptionChainTable(
    const std::shared_ptr<omm::core::models::Market>& market,
    double expiry
) {
    std::vector<OptionChainRow> rows;
    
    double forward = omm::core::Utils::getForwardPrice(market->spot, market->interestRate, expiry);
    std::vector<double> normStrikes = omm::core::Utils::getNormStrikes(
        market->spot,
        market->interestRate,
        expiry,
        market->volSurface->getAtmVol(expiry)
    );
    
    using namespace omm::core::models;
    using namespace omm::core::workers;
    
    for (double ns : normStrikes) {
        double strike = market->volSurface->getStrike(ns, forward, expiry);
        
        auto optionCall = std::make_shared<Option>(
            market->asset,
            strike,
            expiry,
            OptionType::CALL,
            1
        );
        
        auto optionPut = std::make_shared<Option>(
            market->asset,
            strike,
            expiry,
            OptionType::PUT,
            1
        );
        
        double callPrice = Calculator::priceOption(*optionCall, *market);
        Risk callRisk = Calculator::calculateRisk(*optionCall, *market);
        double putPrice = Calculator::priceOption(*optionPut, *market);
        Risk putRisk = Calculator::calculateRisk(*optionPut, *market);
        
        double forwardStrike = strike * std::exp(market->interestRate * expiry / 365.0);
        double impliedVol = market->volSurface->getVolNormStrike(ns, expiry);
        
        OptionChainRow row{
            callRisk.theta / 365.0,           // callTheta per day
            callRisk.vega / 100.0,            // callVega per 1% vol move
            callRisk.gamma,                   // callGamma
            callRisk.delta,                   // callDelta
            callPrice,                        // callPrice
            ns,                               // normStrike
            strike,                           // strike
            forwardStrike,                    // forwardStrike
            impliedVol,                       // impliedVol
            putPrice,                         // putPrice
            putRisk.delta,                    // putDelta
            putRisk.gamma,                    // putGamma
            putRisk.vega / 100.0,             // putVega per 1% vol move
            putRisk.theta / 365.0             // putTheta per day
        };
        
        rows.push_back(row);
    }
    
    return rows;
}

void Table::printOptionChainTable(const std::vector<OptionChainRow>& chain) {
    if (chain.empty()) {
        std::cout << "Empty option chain table\n";
        return;
    }
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << std::string(200, '=') << "\n";
    std::cout << std::setw(12) << "Call Theta"
              << std::setw(12) << "Call Vega"
              << std::setw(12) << "Call Gamma"
              << std::setw(12) << "Call Delta"
              << std::setw(12) << "Call Price"
              << std::setw(12) << "Norm Strike"
              << std::setw(12) << "Strike"
              << std::setw(12) << "Fwd Strike"
              << std::setw(12) << "Impl Vol"
              << std::setw(12) << "Put Price"
              << std::setw(12) << "Put Delta"
              << std::setw(12) << "Put Gamma"
              << std::setw(12) << "Put Vega"
              << std::setw(12) << "Put Theta\n";
    std::cout << std::string(200, '-') << "\n";
    
    for (const auto& row : chain) {
        std::cout << std::setw(12) << row.callTheta
                  << std::setw(12) << row.callVega
                  << std::setw(12) << row.callGamma
                  << std::setw(12) << row.callDelta
                  << std::setw(12) << row.callPrice
                  << std::setw(12) << row.normStrike
                  << std::setw(12) << row.strike
                  << std::setw(12) << row.forwardStrike
                  << std::setw(12) << row.impliedVol
                  << std::setw(12) << row.putPrice
                  << std::setw(12) << row.putDelta
                  << std::setw(12) << row.putGamma
                  << std::setw(12) << row.putVega
                  << std::setw(12) << row.putTheta << "\n";
    }
    std::cout << std::string(200, '=') << "\n";
}

}  // namespace omm::analytics
