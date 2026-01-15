#pragma once

#include <map>

class VolSmile {
public:
    VolSmile(const std::vector<double>& normStrikes, const std::vector<double>& vols);
    double getVol(double normStrike) const;
    std::vector<double> getNormStrikes() const { 
        return normStrikes_; 
    }

private:
    std::map<double, double> smile_;
    std::vector<double> normStrikes_;
    double interpolate(double normStrike) const;

    bool empty() const { 
        return smile_.empty(); 
    }
};

class VolSurface {
public:
    VolSurface(double vol_1m_est, std::vector<double> expiries, double skew, double convexity, double volMean, double spot, double riskFreeRate);
    double getVol(double expiry, double normStrike) const;
    double getVol1mEst() const;

private:
    double vol_1m_est_;
    std::map<double, VolSmile> surface_;
    double interpolate(double expiry, double normStrike) const;
    void hasButterflyArb() const;
    void hasCalendarArb() const;

    bool empty() const { 
        return surface_.empty(); 
    }
};