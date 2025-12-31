#include "core/models/volsurface.hpp"
#include "core/utils.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>

namespace omm::core::models {

double Smile::getVol(double normStrike) const {
    if (normStrike > normStrikes.back()) {
        return volPoints.back();
    } else if (normStrike < normStrikes.front()) {
        return volPoints.front();
    } else {
        // Binary search for insertion point
        auto it = std::lower_bound(normStrikes.begin(), normStrikes.end(), normStrike);
        int idx = std::distance(normStrikes.begin(), it);
        
        double normStrikeUp = normStrikes[idx];
        double normStrikeDown = normStrikes[idx - 1];
        double volPointUp = volPoints[idx];
        double volPointDown = volPoints[idx - 1];
        
        double weight = (normStrikeUp != normStrikeDown) 
            ? (normStrike - normStrikeDown) / (normStrikeUp - normStrikeDown)
            : 0.0;
        
        return volPointDown + weight * (volPointUp - volPointDown);
    }
}

VolSurface::VolSurface(
    const std::vector<double>& expiries_,
    double atmOneMonthVolEst_,
    double skew,
    double convexity,
    double volMean,
    double spot,
    double interestRate
) : atmOneMonthVolEst(atmOneMonthVolEst_) {
    
    expiries = expiries_;
    smiles.resize(expiries_.size());
    
    for (size_t idx = 0; idx < expiries.size(); ++idx) {
        double expiry = expiries[idx];
        
        // ATM vol decays to 1-month vol which then decays to long-running vol
        double weight = std::exp(-std::abs(expiry - 30.0) / 365.0);
        double atmVol = std::sqrt(
            volMean * volMean + 
            weight * (atmOneMonthVolEst_ * atmOneMonthVolEst_ - volMean * volMean)
        );
        
        std::vector<double> normStrikes = Utils::getNormStrikes(spot, interestRate, expiry, atmVol);
        
        for (double ns : normStrikes) {
            double vol = atmVol + skew * ns + convexity * ns * ns;
            addVolPoint(idx, ns, vol);
        }
    }
    
    // Verify no arbitrage (warnings only, don't fail)
    if (hasButterflyArbitrage()) {
        std::cerr << "Warning: Butterfly arbitrage detected in vol surface\n";
    }
    if (hasCalendarArbitrage()) {
        std::cerr << "Warning: Calendar arbitrage detected in vol surface\n";
    }
}

void VolSurface::addVolPoint(int idx, double normStrike, double vol) {
    if (idx >= static_cast<int>(smiles.size())) {
        smiles.resize(idx + 1);
    }
    
    smiles[idx].normStrikes.push_back(normStrike);
    smiles[idx].volPoints.push_back(vol);
}

double VolSurface::getNormStrike(double strike, double forward, double expiry) const {
    double atmVol = getAtmVol(expiry);
    return Utils::getNormStrike(strike, forward, expiry, atmVol);
}

double VolSurface::getStrike(double normStrike, double forward, double expiry) const {
    double atmVol = getAtmVol(expiry);
    return Utils::getStrikeFromNormStrike(normStrike, forward, expiry, atmVol);
}

double VolSurface::getVolNormStrike(double normStrike, double expiry) const {
    if (expiries.empty() || smiles.empty()) {
        return 0.15;  // Default fallback
    }
    
    if (expiry > expiries.back()) {
        return smiles.back().getVol(normStrike);
    } else if (expiry < expiries.front()) {
        return smiles.front().getVol(normStrike);
    } else {
        auto it = std::lower_bound(expiries.begin(), expiries.end(), expiry);
        int idx = std::distance(expiries.begin(), it);
        
        if (idx >= static_cast<int>(smiles.size())) {
            idx = smiles.size() - 1;
        }
        if (idx <= 0) {
            idx = 1;
        }
        
        double expiryUp = expiries[idx];
        double expiryDown = expiries[idx - 1];
        double volUp = smiles[idx].getVol(normStrike);
        double volDown = smiles[idx - 1].getVol(normStrike);
        
        double weight = (expiryUp != expiryDown)
            ? (expiry - expiryDown) / (expiryUp - expiryDown)
            : 0.0;
        
        return volDown + weight * (volUp - volDown);
    }
}

double VolSurface::getVol(double strike, double forward, double expiry) const {
    double normStrike = getNormStrike(strike, forward, expiry);
    return getVolNormStrike(normStrike, expiry);
}

double VolSurface::getAtmVol(double expiry) const {
    return getVolNormStrike(0.0, expiry);
}

bool VolSurface::hasButterflyArbitrage() const {
    for (size_t idx = 0; idx < expiries.size(); ++idx) {
        const Smile& smile = smiles[idx];
        double T = expiries[idx] / 365.0;
        
        if (smile.normStrikes.size() < 3) continue;
        
        // Check variance convexity (not IV convexity)
        for (size_t i = 1; i + 1 < smile.volPoints.size(); ++i) {
            double var_down = smile.volPoints[i - 1] * smile.volPoints[i - 1] * T;
            double var_mid = smile.volPoints[i] * smile.volPoints[i] * T;
            double var_up = smile.volPoints[i + 1] * smile.volPoints[i + 1] * T;
            
            double var_diff = var_up - 2.0 * var_mid + var_down;
            if (var_diff < -1e-8) {
                return true;
            }
        }
    }
    return false;
}

bool VolSurface::hasCalendarArbitrage() const {
    for (size_t i = 0; i + 1 < expiries.size(); ++i) {
        double expiryEarlier = expiries[i];
        double expiryLater = expiries[i + 1];
        const Smile& smileEarlier = smiles[i];
        const Smile& smileLater = smiles[i + 1];
        
        // Find common normalized strikes (rounded to 2 decimals)
        std::vector<double> commonStrikes;
        for (double ns : smileEarlier.normStrikes) {
            double nsRounded = std::round(ns * 100.0) / 100.0;
            for (double nsLater : smileLater.normStrikes) {
                double nsLaterRounded = std::round(nsLater * 100.0) / 100.0;
                if (std::abs(nsRounded - nsLaterRounded) < 1e-6) {
                    commonStrikes.push_back(ns);
                    break;
                }
            }
        }
        
        for (double ns : commonStrikes) {
            double volEarlier = smileEarlier.getVol(ns);
            double volLater = smileLater.getVol(ns);
            
            double varEarlier = volEarlier * volEarlier * expiryEarlier / 365.0;
            double varLater = volLater * volLater * expiryLater / 365.0;
            
            if (varLater < varEarlier - 1e-8) {
                return true;
            }
        }
    }
    return false;
}

}  // namespace omm::core::models
