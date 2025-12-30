import numpy as np

from src.core.models.regime import Regime
from src.core.models.regimeparams import RegimeParams

TIME_STEP = 1 # 1 = 1day

SPX_SPOT = 25000.0
VIX = 0.22
INTEREST_RATE = 0.05

EXPIRIES = np.array([4 + 7 * i for i in range(26)])

REGIME_TRANSITION = np.array([
    [0.97, 0.03,  0.00],   # From CALM
    [0.15, 0.80,  0.05],   # From STRESS
    [0.00, 0.20,  0.80],   # From EVENT
])

REGIME_PARAMS = {
    Regime.CALM: RegimeParams(
        spot_vol=0.12,
        vol_mean=0.18,
        vol_kappa=4.0,
        vol_of_vol=0.20,
        rho=-0.3,
        skew=-0.02,
        convexity=0.01
    ),
    Regime.STRESS: RegimeParams(
        spot_vol=0.25,
        vol_mean=0.28,
        vol_kappa=1.5,
        vol_of_vol=0.45,
        rho=-0.6,
        skew=-0.05,
        convexity=0.02
    ),
    Regime.EVENT: RegimeParams(
        spot_vol=0.40,
        vol_mean=0.35,
        vol_kappa=0.5,
        vol_of_vol=0.80,
        rho=-0.75,
        skew=-0.08,
        convexity=0.03
    ),
}