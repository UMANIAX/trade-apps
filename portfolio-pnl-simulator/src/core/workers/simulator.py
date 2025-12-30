
import numpy as np
from src.core.models.asset import Asset
from src.core.models.volsurface import VolSurface
from src.core.config import EXPIRIES, INTEREST_RATE, REGIME_PARAMS, REGIME_TRANSITION, SPX_SPOT, TIME_STEP, VIX
from src.core.models.market import Market
from src.core.models.regimeparams import RegimeParams
from src.core.models.regime import Regime

def initialize_market() -> Market:
    asset = Asset(symbol='.NDX')
    market = Market(
        asset=asset,
        time=-TIME_STEP,
        spot=SPX_SPOT,
        vol_surface=None,
        interest_rate=INTEREST_RATE,
        regime=Regime.CALM
    )
    return simulate_next_market(market, VIX, EXPIRIES + TIME_STEP)

def get_next_regime(regime: Regime):    
    probs = REGIME_TRANSITION[regime.value]
    return Regime(np.random.choice(len(probs), p=probs))

def simulate_next_market(market: Market, atm_1m_vol: float = None, expiries: float = None) -> Market:
    current_time = market.time
    dt = TIME_STEP/365
    # new expiries
    expiries = market.vol_surface.expiries if expiries is None else expiries
    new_expiries = expiries - TIME_STEP
    new_expiries = new_expiries[new_expiries > 0]
    num_expiries_removed = len(expiries) - len(new_expiries)
    # remove -ve expiries and add new ones at the end with step 7 (weekly)
    expiries_to_add = np.array([new_expiries[-1] + 7 * (i + 1) for i in range(num_expiries_removed)])
    new_expiries = np.concatenate([new_expiries[num_expiries_removed:], expiries_to_add])
    # spot and atm_vol (correlated) shocks
    next_regime = get_next_regime(market.regime)
    regime_params = REGIME_PARAMS[next_regime]
    z1 = np.random.normal()
    z2 = np.random.normal()
    z_spot = z1
    z_vol = regime_params.rho * z1 + np.sqrt(1 - regime_params.rho**2) * z2
    # spot GBM: dS(t) = σS(t)dW --Ito-Calculus--> s(t+1) = s(t) * e^(-σ^2*dt/2 + σ*dW) 
    spot_next = market.spot * np.exp(-0.5 * regime_params.spot_vol**2 * dt + regime_params.spot_vol * np.sqrt(dt) * z_spot)
    # vol Ornstein-Uhlenbeck process
    atm_1m_vol = market.vol_surface.atm_1m_vol_est if atm_1m_vol is None else atm_1m_vol
    atm_1m_vol_next = atm_1m_vol + regime_params.vol_kappa * (regime_params.vol_mean - atm_1m_vol) * dt + regime_params.vol_of_vol * np.sqrt(dt) * z_vol
    vs_next = VolSurface(expiries=new_expiries, atm_1m_vol_est=atm_1m_vol_next, skew=regime_params.skew, convexity=regime_params.convexity, 
                         vol_mean=regime_params.vol_mean, spot=spot_next, interest_rate=market.interest_rate)
    return Market(market.asset, current_time + TIME_STEP, spot_next, vs_next, market.interest_rate, regime=next_regime)