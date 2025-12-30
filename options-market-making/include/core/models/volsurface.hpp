#pragma once

#include <vector>
#include <map>

namespace omm::core::models {

class Smile {
public:
    std::vector<double> normStrikes;
    std::vector<double> volPoints;
    
    Smile() = default;
    Smile(const std::vector<double>& ns, const std::vector<double>& vp)
        : normStrikes(ns), volPoints(vp) {}
    
    double getVol(double normStrike) const;
};

class VolSurface {
public:
    std::vector<double> expiries;
    std::vector<Smile> smiles;
    double atmOneMonthVolEst;
    
    VolSurface() = default;
    
    VolSurface(
        const std::vector<double>& expiries_,
        double atmOneMonthVolEst_,
        double skew,
        double convexity,
        double volMean,
        double spot,
        double interestRate
    );
    
    void addVolPoint(int idx, double normStrike, double vol);
    double getNormStrike(double strike, double forward, double expiry) const;
    double getStrike(double normStrike, double forward, double expiry) const;
    double getVolNormStrike(double normStrike, double expiry) const;
    double getVol(double strike, double forward, double expiry) const;
    double getAtmVol(double expiry) const;
    
    bool hasButterflyArbitrage() const;
    bool hasCalendarArbitrage() const;
};

}  // namespace omm::core::models
