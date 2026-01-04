#pragma once

enum class RegimeType {
    CALM,
    STRESS,
    EVENT,
    COUNT // just for counter
};

struct RegimeParams {
    double spotDrift;
    double spotVol;
    double volOfVol;
    double volKappa; // mean-reversion speed
    double volMean;
    double rho; // spot-vol corr
    double skew;
    double convexity;

    RegimeParams(
        double spotDrift_ = 0.0,
        double spotVol_ = 0.0,
        double volOfVol_ = 0.0,
        double volKappa_ = 0.0,
        double volMean_ = 0.0,
        double rho_ = 0.0,
        double skew_ = 0.0,
        double convexity_ = 0.0
    ) : 
        spotDrift(spotDrift_), 
        spotVol(spotVol_), 
        volOfVol(volOfVol_),
        volKappa(volKappa_), 
        volMean(volMean_), 
        rho(rho_),
        skew(skew_), 
        convexity(convexity_) {}
};

struct Regime {
    RegimeType type;
    RegimeParams params;

    Regime(
        RegimeType type_,
        RegimeParams params_
    )
        : 
        type(type_), 
        params(params_) {}
};