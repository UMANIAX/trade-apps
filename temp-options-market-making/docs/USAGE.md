# Options Market Making - Quick Start Guide

## Building

```bash
cd /Users/umangpahwa/Projects/trade-apps/options-market-making
make
```

## Running

```bash
./options-market-making
```

## Sample Output

```
Initializing options market simulator...

Market initialized successfully!
Spot Price: 25150.9
Current Regime: 0
ATM 1M Vol: 0.21885
Number of Expiries: 26

Option Pricing for first expiry (4 days):
----------------------------------------------------------------------------------------------------
      Strike  Call Price   Put Price          IV  Call Delta  Put Delta
----------------------------------------------------------------------------------------------------
  23893.3543   1290.5481     19.9145      0.3145      0.9437     -0.0558
  25150.8993    234.7074    220.9298      0.2169      0.5139     -0.4856
  26408.4442      3.7954   1246.8739      0.2192      0.0182     -0.9812
----------------------------------------------------------------------------------------------------

Simulation complete!
```

## C++ API Examples

### Initialize Market

```cpp
#include "core/workers/simulator.hpp"

auto market = omm::core::workers::Simulator::initializeMarket();
std::cout << "Spot: " << market->spot << "\n";
std::cout << "Vol: " << market->volSurface->atmOneMonthVolEst << "\n";
```

### Simulate Market Movement

```cpp
auto nextMarket = omm::core::workers::Simulator::simulateNextMarket(market);
std::cout << "New Spot: " << nextMarket->spot << "\n";
```

### Price an Option

```cpp
#include "core/workers/calculator.hpp"
#include "core/models/option.hpp"

using namespace omm::core::models;
auto option = std::make_shared<Option>(market->asset, 25000, 30, OptionType::CALL, 1);
double price = omm::core::workers::Calculator::priceOption(*option, *market);
std::cout << "Call Price: " << price << "\n";
```

### Calculate Greeks

```cpp
auto risk = omm::core::workers::Calculator::calculateRisk(*option, *market);
std::cout << "Delta: " << risk.delta << "\n";
std::cout << "Gamma: " << risk.gamma << "\n";
std::cout << "Vega: " << risk.vega << "\n";
std::cout << "Theta: " << risk.theta << "\n";
```

### Monte Carlo Simulation

```cpp
std::vector<std::shared_ptr<Market>> paths;
auto m = Simulator::initializeMarket();

for (int i = 0; i < 1000; ++i) {
    m = Simulator::simulateNextMarket(m);
    paths.push_back(m);
}
```

## Files Structure

- `include/core/config.hpp` - Configuration parameters
- `include/core/utils.hpp` - Utility functions
- `include/core/models/` - Data structures (Option, Market, etc.)
- `include/core/workers/` - Simulators and calculators
- `include/analytics/` - Analysis and visualization functions
- `src/` - Implementation files

## Performance

Typical execution times on modern hardware:

- Option pricing: < 1 microsecond
- Greeks calculation: < 5 microseconds
- Market simulation step: < 100 microseconds
- Full simulation (10 steps): < 1 millisecond

## Customization

Edit `include/core/config.hpp` to change:

- Initial spot price: `SPX_SPOT`
- Initial volatility: `VIX`
- Risk-free rate: `INTEREST_RATE`
- Expiries: `getExpiries()`
- Regime parameters in `src/core/workers/simulator.cpp`

## Common Issues

### Build fails - compiler not found
Install a C++17 compiler:
- macOS: `brew install llvm`
- Ubuntu: `sudo apt-get install g++`
- Windows: Install Visual Studio 2017 or later

### Segmentation fault
Ensure market is properly initialized:
```cpp
if (!market || !market->volSurface) {
    std::cerr << "Market initialization failed\n";
}
```

### Slow execution
Make sure optimization flags are enabled:
```bash
make CXXFLAGS="-std=c++17 -O3 -march=native -I./include"
```

## Next Steps

1. Extend the `analytics/` module for visualization
2. Add support for American options (binomial trees)
3. Implement portfolio hedging strategies
4. Add multi-threaded Monte Carlo simulation
5. Create REST API interface

## Support

For detailed documentation, see [README.md](README.md)
