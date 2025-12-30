import numpy as np

def get_norm_strike(strike: float, forward: float, expiry: float, atm_vol: float) -> float:
    return np.log(strike / forward) / (atm_vol * np.sqrt(expiry / 365))

def get_strike_from_norm_strike(norm_strike: float, forward: float, expiry: float, atm_vol: float) -> float:
    return forward * np.exp(norm_strike * atm_vol * np.sqrt(expiry / 365))

def get_norm_strikes(spot: float, interest_rate: float, expiry: float, atm_vol: float, max_strike_step_dist: int = 40, strike_step: int = 50.0):
    forward = get_forward_price(spot, interest_rate, expiry)
    norm_strikes = []
    for z in range(-max_strike_step_dist, max_strike_step_dist + 1):
        strike = spot + z * strike_step
        ns = get_norm_strike(strike, forward, expiry, atm_vol)
        norm_strikes.append(ns)
    return norm_strikes

def get_forward_price(spot: float, interest_rate: float, expiry: float) -> float:
    tte = expiry / 365
    return spot * np.exp(interest_rate * tte)