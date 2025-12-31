#pragma once

namespace omm::core::models {

struct Risk {
    double delta;
    double gamma;
    double vega;
    double theta;
    
    Risk(double delta_ = 0.0, double gamma_ = 0.0, double vega_ = 0.0, double theta_ = 0.0)
        : delta(delta_), gamma(gamma_), vega(vega_), theta(theta_) {}
};

}  // namespace omm::core::models
