#pragma once

#include <map>

class VolSmile {
public:
    void addPoint(double normStrike, double vol);
    double getVol(double normStrike) const;

    bool empty() const { 
        return smile_.empty(); 
    }

private:
    std::map<double, double> smile_;
    double interpolate(double normStrike) const;
};

class VolSurface {
public:
    void addSmile(double expiry, const VolSmile& smile);
    double getVol(double expiry, double normStrike) const;
    
    bool empty() const { 
        return surface_.empty(); 
    }

private:
    std::map<double, VolSmile> surface_;
    double interpolate(double expiry, double normStrike) const;
};