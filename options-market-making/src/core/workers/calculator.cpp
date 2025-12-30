#include "core/workers/calculator.hpp"
#include "core/utils.hpp"
#include <cmath>

namespace omm::core::workers {

constexpr double PI = 3.14159265358979323846;
constexpr double SQRT_2PI = 2.50662827463100050241;

// Standard normal CDF
static double normCdf(double x) {
    return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}

// Standard normal PDF
static double normPdf(double x) {
    return std::exp(-0.5 * x * x) / SQRT_2PI;
}

double Calculator::priceFuture(const omm::core::models::Future& future, const omm::core::models::Market& market) {
    double tte = std::stod(future.expiry) / 365.0;
    return Utils::getForwardPrice(market.spot, market.interestRate, std::stod(future.expiry));
}

OptionPricerInputs Calculator::getOptionPricerInputs(
    const omm::core::models::Option& option,
    const omm::core::models::Market& market
) {
    double tte = option.expiry / 365.0;
    double forward = market.spot * std::exp(market.interestRate * tte);
    double sigma = market.volSurface->getVol(option.strike, forward, option.expiry);
    
    double d1 = (std::log(forward / option.strike) + 0.5 * sigma * sigma * tte) / (sigma * std::sqrt(tte));
    double d2 = d1 - sigma * std::sqrt(tte);
    double df = std::exp(-market.interestRate * tte);
    
    return OptionPricerInputs{tte, forward, sigma, d1, d2, df};
}

double Calculator::priceOption(const omm::core::models::Option& option, const omm::core::models::Market& market) {
    OptionPricerInputs inputs = getOptionPricerInputs(option, market);
    
    if (option.optionType == omm::core::models::OptionType::CALL) {
        return inputs.df * (inputs.forward * normCdf(inputs.d1) - option.strike * normCdf(inputs.d2));
    } else {
        return inputs.df * (option.strike * normCdf(-inputs.d2) - inputs.forward * normCdf(-inputs.d1));
    }
}

omm::core::models::Risk Calculator::calculateRisk(
    const omm::core::models::Option& option,
    const omm::core::models::Market& market
) {
    OptionPricerInputs inputs = getOptionPricerInputs(option, market);
    
    double delta, theta;
    if (option.optionType == omm::core::models::OptionType::CALL) {
        delta = inputs.df * normCdf(inputs.d1);
    } else {
        delta = -inputs.df * normCdf(-inputs.d1);
    }
    
    double gamma = inputs.df * normPdf(inputs.d1) / (inputs.forward * inputs.sigma * std::sqrt(inputs.tte));
    double vega = inputs.df * inputs.forward * normPdf(inputs.d1) * std::sqrt(inputs.tte);
    
    double thetaTime = -inputs.df * inputs.forward * normPdf(inputs.d1) * inputs.sigma / (2.0 * std::sqrt(inputs.tte));
    double thetaCarry;
    if (option.optionType == omm::core::models::OptionType::CALL) {
        thetaCarry = -market.interestRate * inputs.df * inputs.forward * normCdf(inputs.d1);
    } else {
        thetaCarry = market.interestRate * inputs.df * inputs.forward * normCdf(-inputs.d1);
    }
    theta = thetaTime + thetaCarry;
    
    return omm::core::models::Risk(delta, gamma, vega, theta);
}

omm::core::models::Risk Calculator::calculatePortfolioRisk(
    const std::vector<std::shared_ptr<omm::core::models::Security>>& positions,
    const omm::core::models::Market& market
) {
    double totalDelta = 0.0, totalGamma = 0.0, totalVega = 0.0, totalTheta = 0.0;
    
    for (const auto& position : positions) {
        // Skip null positions
        if (!position) continue;
        
        // Try to cast as Option
        auto* optPtr = dynamic_cast<omm::core::models::Option*>(position.get());
        if (optPtr != nullptr) {
            omm::core::models::Risk risk = calculateRisk(*optPtr, market);
            totalDelta += risk.delta * optPtr->lotSize;
            totalGamma += risk.gamma * optPtr->lotSize;
            totalVega += risk.vega * optPtr->lotSize;
            totalTheta += risk.theta * optPtr->lotSize;
            continue;
        }
        
        // Try to cast as Future
        auto* futPtr = dynamic_cast<omm::core::models::Future*>(position.get());
        if (futPtr != nullptr) {
            totalDelta += 1.0 * futPtr->lotSize;
        }
    }
    
    return omm::core::models::Risk(totalDelta, totalGamma, totalVega, totalTheta);
}

double Calculator::getForwardStrike(const omm::core::models::Market& market, double strike, double expiry) {
    double tte = expiry / 365.0;
    return strike * std::exp(market.interestRate * tte);
}

}  // namespace omm::core::workers
