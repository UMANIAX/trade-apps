#include "VolSurface.h"
#include <stdexcept>

void VolSmile::addPoint(double normStrike, double vol) {
    if (vol <= 0.0) {
        throw std::invalid_argument("Vol must be positive");
    }
    smile_[normStrike] = vol;
};

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

void VolSurface::addSmile(double expiry, const VolSmile& smile) {
    if (expiry <= 0.0) {
        throw std::invalid_argument("Expiry most be positive");
    }
    if (smile.empty()) {
        throw std::invalid_argument("Smile cannot be empty");
    }
    surface_[expiry] = smile;
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

