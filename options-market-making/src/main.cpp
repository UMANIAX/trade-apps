#include "core/config.hpp"
#include "core/utils.hpp"
#include "core/workers/simulator.hpp"
#include "core/workers/calculator.hpp"
#include <iostream>
#include <memory>
#include <iomanip>

int main() {
    using namespace omm::core::workers;
    using namespace omm::core::models;
    using namespace omm::core;
    
    try {
        std::cout << "Initializing options market simulator...\n" << std::endl;
        
        // Initialize market
        auto market = Simulator::initializeMarket();
        
        if (!market || !market->volSurface) {
            std::cerr << "Failed to initialize market\n";
            return 1;
        }
        
        std::cout << "Market initialized successfully!\n";
        std::cout << "Spot Price: " << market->spot << "\n";
        std::cout << "Current Regime: " << static_cast<int>(market->regime) << "\n";
        std::cout << "ATM 1M Vol: " << market->volSurface->atmOneMonthVolEst << "\n";
        std::cout << "Number of Expiries: " << market->volSurface->expiries.size() << "\n\n";
        
        // Get first expiry and generate some sample prices
        if (!market->volSurface->expiries.empty()) {
            double expiry = market->volSurface->expiries[0];
            double forward = Utils::getForwardPrice(market->spot, market->interestRate, expiry);
            
            std::cout << "Option Pricing for first expiry (" << expiry << " days):\n";
            std::cout << std::string(100, '-') << "\n";
            std::cout << std::setw(12) << "Strike"
                      << std::setw(12) << "Call Price"
                      << std::setw(12) << "Put Price"
                      << std::setw(12) << "IV"
                      << std::setw(12) << "Call Delta"
                      << std::setw(12) << "Put Delta\n";
            std::cout << std::string(100, '-') << "\n";
            std::cout << std::fixed << std::setprecision(4);
            
            // Price options at ATM and nearby strikes
            std::vector<double> strikes = {market->spot * 0.95, market->spot, market->spot * 1.05};
            
            for (double strike : strikes) {
                auto call = std::make_shared<Option>(market->asset, strike, expiry, OptionType::CALL, 1);
                auto put = std::make_shared<Option>(market->asset, strike, expiry, OptionType::PUT, 1);
                
                double callPrice = Calculator::priceOption(*call, *market);
                double putPrice = Calculator::priceOption(*put, *market);
                double iv = market->volSurface->getVol(strike, forward, expiry);
                auto callRisk = Calculator::calculateRisk(*call, *market);
                auto putRisk = Calculator::calculateRisk(*put, *market);
                
                std::cout << std::setw(12) << strike
                          << std::setw(12) << callPrice
                          << std::setw(12) << putPrice
                          << std::setw(12) << iv
                          << std::setw(12) << callRisk.delta
                          << std::setw(12) << putRisk.delta << "\n";
            }
            
            std::cout << std::string(100, '-') << "\n";
        }
        
        std::cout << "\nSimulation complete!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred\n";
        return 1;
    }
    
    return 0;
}
