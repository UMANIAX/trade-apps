#pragma once

#include "core/models/market.hpp"
#include "core/models/option.hpp"
#include "core/models/future.hpp"
#include "core/models/risk.hpp"
#include <vector>
#include <memory>

namespace omm::core::workers {

struct OptionPricerInputs {
    double tte;
    double forward;
    double sigma;
    double d1;
    double d2;
    double df;
};

class Calculator {
public:
    // Price a future contract
    static double priceFuture(const omm::core::models::Future& future, const omm::core::models::Market& market);
    
    // Get option pricer inputs (Black-Scholes parameters)
    static OptionPricerInputs getOptionPricerInputs(const omm::core::models::Option& option, const omm::core::models::Market& market);
    
    // Price an option using Black-Scholes
    static double priceOption(const omm::core::models::Option& option, const omm::core::models::Market& market);
    
    // Calculate Greeks for an option
    static omm::core::models::Risk calculateRisk(const omm::core::models::Option& option, const omm::core::models::Market& market);
    
    // Calculate portfolio-level Greeks
    static omm::core::models::Risk calculatePortfolioRisk(
        const std::vector<std::shared_ptr<omm::core::models::Security>>& positions,
        const omm::core::models::Market& market
    );
    
private:
    static double getForwardStrike(const omm::core::models::Market& market, double strike, double expiry);
};

}  // namespace omm::core::workers
