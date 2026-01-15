#include "VolSurface.h"
#include "Helper.h"
#include <stdexcept>

VolSmile::VolSmile(const std::vector<double>& normStrikes, const std::vector<double>& vols) {
    if (normStrikes.size() != vols.size()) {
        throw std::invalid_argument("normStrikes and vols must have same size");
    }
    for (size_t i = 0; i < normStrikes.size(); ++i) {
        if (vols[i] <= 0.0) {
            throw std::invalid_argument("Vol must be positive");
        }
        smile_[normStrikes[i]] = vols[i];
    }
    normStrikes_ = normStrikes;
}

double VolSmile::getVol(double normStrike) const {
    if (smile_.empty()) {
        throw std::runtime_error("VolSmile not initialized");
    }

    auto smileIter = smile_.lower_bound(normStrike);

    if (smileIter != smile_.end() && smileIter->first == normStrike) {
        return smileIter->second;
    }
    if (smileIter == smile_.begin()) {
        return smileIter->second;
    }
    if (smileIter == smile_.end()) {
        return std::prev(smileIter)->second;
    }
    return interpolate(normStrike);
}

double VolSmile::interpolate(double normStrike) const {
    auto smileIterUp = smile_.lower_bound(normStrike);
    auto smileIterDown = std::prev(smileIterUp);

    double normStrikeUp = smileIterUp->first;
    double normStrikeDown = smileIterDown->first;
    double volUp = smileIterUp->second;
    double volDown = smileIterDown->second;

    double weight = (normStrike - normStrikeDown) / (normStrikeUp - normStrikeDown);
    return volDown + weight * (volUp - volDown);
}

VolSurface::VolSurface(double vol_1m_est, std::vector<double> expiries, double skew, double convexity, double volMean, double spot, double riskFreeRate) {
    vol_1m_est_ = vol_1m_est;
    for (double expiry: expiries) {
        // atm_var decays to atm_1m_var which then decays to long running var
        double weight = std::exp(-std::abs(expiry - 30.0) / 365.0);
        double atm_vol = std::sqrt(std::pow(volMean, 2) + weight * (std::pow(vol_1m_est, 2) - std::pow(volMean, 2)));
        std::vector<double> normStrikes = getNormStrikesForExpiry(spot, riskFreeRate, expiry, atm_vol, 100, 10);
        std::vector<double> vols;
        for (double ns: normStrikes) {
            double vol = atm_vol * (1 + skew * ns + convexity * ns * ns);
            vols.push_back(vol);
        }
        surface_[expiry] = VolSmile(normStrikes, vols);
    }
    hasButterflyArb();
    hasCalendarArb();
}

double VolSurface::getVol1mEst() const {
    return vol_1m_est_;
}

double VolSurface::getVol(double expiry, double normStrike) const {
    auto surfaceIter = surface_.lower_bound(normStrike);

    if (surfaceIter != surface_.end() && surfaceIter->first == normStrike) {
        return surfaceIter->second.getVol(normStrike);
    }
    if (surfaceIter == surface_.begin()) {
        return surfaceIter->second.getVol(normStrike);;
    }
    if (surfaceIter == surface_.end()) {
        return std::prev(surfaceIter)->second.getVol(normStrike);;
    }
    return interpolate(expiry, normStrike);
}

double VolSurface::interpolate(double expiry, double normStrike) const {
    auto surfaceIterUp = surface_.lower_bound(normStrike);
    auto surfaceIterDown = std::prev(surfaceIterUp);

    double expiryUp = surfaceIterUp->first;
    double expiryDown = surfaceIterDown->first;
    double volUp = surfaceIterUp->second.getVol(normStrike);
    double volDown = surfaceIterDown->second.getVol(normStrike);

    double weight = (expiry - expiryDown) / (expiryUp - expiryDown);
    return volDown + weight * (volUp - volDown);
}

// Check for Varriance instead of Price convexity
// Reason: Intrinsic Value cancels out in buttefly spreads
// So only worth checking for time values (or variance)
// Why variance and not IV?
// Black Scholes uses Variance (σ^2*T) to price (time-value) not IV
void VolSurface::hasButterflyArb() const {
    for (const auto& [expiry, smile] : surface_) {
        double tte = expiry / 365.0;
        std::vector<double> normStrikes = smile.getNormStrikes();
        for (int i = 1; i < smile.getNormStrikes().size() - 1; ++i) {
            double ns1 = normStrikes[i - 1];
            double ns2 = normStrikes[i];
            double ns3 = normStrikes[i + 1];
            double var1 = std::pow(smile.getVol(ns1), 2) * tte;
            double var2 = std::pow(smile.getVol(ns2), 2) * tte;
            double var3 = std::pow(smile.getVol(ns3), 2) * tte;
            double varDiff = var3 - 2 * var2 + var1;
            if (varDiff < 0) {
                throw std::runtime_error("Butterfly Arbitrage Detected: "
                    "expiry " + std::to_string(expiry) + ", normStrikes " + std::to_string(ns1) + ", " + std::to_string(ns2) + ", " + std::to_string(ns3));
            }
        }
    }
}

void VolSurface::hasCalendarArb() const {
    auto iter1 = surface_.begin();
    auto iter2 = std::next(iter1);
    while (iter2 != surface_.end()) {
        double expiry1 = iter1->first;
        double expiry2 = iter2->first;
        double tte1 = expiry1 / 365.0;
        double tte2 = expiry2 / 365.0;
        const VolSmile& smile1 = iter1->second;
        const VolSmile& smile2 = iter2->second;

        std::vector<double> normStrikes1;
        for (double ns : smile1.getNormStrikes()) {
            normStrikes1.push_back(std::round(ns * 100.0) / 100.0);
        }
        std::vector<double> normStrikes2;
        for (double ns : smile2.getNormStrikes()) {
            normStrikes2.push_back(std::round(ns * 100.0) / 100.0);
        }
        std::vector<double> commonNormStrikes;
        std::set_intersection(normStrikes1.begin(), normStrikes1.end(), normStrikes2.begin(), normStrikes2.end(), std::back_inserter(commonNormStrikes));
        for (double ns : commonNormStrikes) {
            double var1 = std::pow(smile1.getVol(ns), 2) * tte1;
            double var2 = std::pow(smile2.getVol(ns), 2) * tte2;
            if (var2 < var1) {
                throw std::runtime_error("Calendar Arbitrage Detected: "
                    "expiries " +  std::to_string(expiry1) + ", " + std::to_string(expiry2) + ", normStrike " + std::to_string(ns));
            }
        }
    }
}
