from dataclasses import dataclass
import numpy as np

from src.core.utils import get_norm_strike, get_norm_strikes, get_strike_from_norm_strike

@dataclass
class Smile:
    norm_strikes: np.ndarray
    vol_points: np.ndarray

    def get_vol(self, norm_strike: float) -> float:
        if norm_strike > self.norm_strikes[-1]:
            return self.vol_points[-1]
        elif norm_strike < self.norm_strikes[0]:
            return self.vol_points[0]
        else:
            idx = np.searchsorted(self.norm_strikes, norm_strike)
            norm_strike_up = self.norm_strikes[idx]
            norm_strike_down = self.norm_strikes[idx - 1]
            vol_point_up = self.vol_points[idx]
            vol_point_down = self.vol_points[idx - 1]
            weight = (norm_strike - norm_strike_down) / (norm_strike_up - norm_strike_down) if norm_strike_up != norm_strike_down else 0
            return vol_point_down + weight * (vol_point_up - vol_point_down)

@dataclass
class VolSurface:
    surface: dict[int, Smile]
    atm_1m_vol_est: float # 1-month estimate -> something like a VIX

    def __init__(self, atm_1m_vol_est: float, expiries: np.array, skew: float, convexity: float, vol_mean: float, spot: float, interest_rate: float):
        self.atm_1m_vol_est = atm_1m_vol_est
        self.surface = {}
        for expiry in expiries:
            # atm_var decays to atm_1m_var which then decays to long running var
            weight = np.exp(-abs(expiry - 30)/365)
            atm_vol = np.sqrt(vol_mean**2 + weight * (atm_1m_vol_est**2 - vol_mean**2))
            norm_strikes = get_norm_strikes(spot, interest_rate, expiry, atm_vol)
            for ns in norm_strikes:
                vol = atm_vol + skew * ns + convexity * ns**2
                self.add_vol_point(expiry, ns, vol)
        assert not self.has_butterfly_arbitrage()
        assert not self.has_calendar_arbitrage()

    def add_vol_point(self, expiry: int, norm_strike: float, vol: float):
        if expiry not in self.surface:
            self.surface[expiry] = Smile(norm_strikes=np.array([]), vol_points=np.array([]))
        smile = self.surface[expiry]
        # assumption is that points are added in sorted order
        smile.norm_strikes = np.append(smile.norm_strikes, norm_strike)
        smile.vol_points = np.append(smile.vol_points, vol)

    def get_norm_strike(self, strike: float, forward: float, expiry: int) -> float:
        smile = self.surface.get(expiry)
        atm_vol = smile.get_vol(0)
        return get_norm_strike(strike, forward, expiry, atm_vol)
    
    def get_strike(self, norm_strike: float, forward: float, expiry: int) -> float:
        smile = self.surface.get(expiry)
        atm_vol = smile.get_vol(0)
        return get_strike_from_norm_strike(norm_strike, forward, expiry, atm_vol)

    def get_vol_ns(self, norm_strike: float, expiry: int) -> float:
        smile = self.surface.get(expiry)
        return smile.get_vol(norm_strike)

    def get_vol(self, strike: float, forward: float, expiry: int) -> float:
        norm_strike = self.get_norm_strike(strike, forward, expiry)
        return self.get_vol_ns(norm_strike, expiry)
    
    def get_atm_vol(self, expiry: int) -> float:
        smile = self.surface.get(expiry)
        return smile.get_vol(0)
    
    def has_butterfly_arbitrage(self):
        arb_dict = {}
        no_arb = True
        for expiry, smile in self.surface.items():
            T = expiry / 365.0
            norm_strikes = smile.norm_strikes
            vols = smile.vol_points
            vars = (vols ** 2) * T
            # Check for Varriance instead of Price convexity
            # Reason: Intrinsic Value cancels out in buttefly spreads
            # So only worth checking for time values (or variance)
            var_diffs = vars[2:] - 2 * vars[1:-1] + vars[:-2]
            # Why variance and not IV?å
            # Black Scholes uses Variance (σ^2*T) to price (time-value) not IV
            if np.any(var_diffs < -1e-8):
                no_arb = False
                idx = np.where(var_diffs < -1e-8)[0][0] + 1
                arb_dict[expiry] = [True, norm_strikes[idx]]
        return False if no_arb else arb_dict
    
    def has_calendar_arbitrage(self):
        arb_dict = {}
        no_arb = True
        expiries = sorted(self.surface.keys())
        for i in range(len(expiries) - 1):
            expiry_earlier = expiries[i]
            expiry_later = expiries[i + 1]
            smile_earlier = self.surface[expiry_earlier]
            smile_later = self.surface[expiry_later]
            calendar_arb = False
            #r round norm_strikes to 2 decimal places before the intersect
            norm_strikes_earlier = np.round(smile_earlier.norm_strikes, 2)
            norm_strikes_later = np.round(smile_later.norm_strikes, 2)
            norm_strikes = np.intersect1d(norm_strikes_earlier, norm_strikes_later)
            for ns in norm_strikes:
                vol_earlier = smile_earlier.get_vol(ns)
                vol_later = smile_later.get_vol(ns)
                var_earlier = vol_earlier ** 2 * expiry_earlier / 365
                var_later = vol_later ** 2 * expiry_later / 365
                if var_later < var_earlier - 1e-8:
                    no_arb = False
                    calendar_arb = [True, ns]
                    break
            arb_dict[(expiry_earlier, expiry_later)] = calendar_arb
        return False if no_arb else arb_dict