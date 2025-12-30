from dataclasses import dataclass

@dataclass
class RegimeParams:
    spot_vol: float
    vol_mean: float
    vol_kappa: float
    vol_of_vol: float
    rho: float # spot-vol corr
    skew: float
    convexity: float