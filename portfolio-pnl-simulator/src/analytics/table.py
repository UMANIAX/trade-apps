import numpy as np
import pandas as pd

from src.core.utils import get_norm_strikes
from src.core.models.market import Market
from src.core.models.option import Option
from src.core.models.optiontype import OptionType
from src.core.workers.calculator import calculate_risk, get_forward_price, get_forward_strike, price_option

def get_option_chain_table(market: Market, expiry: float) -> pd.DataFrame:
    strikes = []
    fwd_strikes = []
    call_prices = []
    call_deltas = []
    call_gammas = []
    call_vegas = []
    call_thetas = []
    put_prices = []
    put_deltas = []
    put_gammas = []
    put_vegas = []
    put_thetas = []
    ivs = []
    forward = get_forward_price(market.spot, market.interest_rate, expiry)
    norm_strikes = get_norm_strikes(market, expiry, market.vol_surface.get_atm_vol(expiry))
    for ns in norm_strikes:
        strike = market.vol_surface.get_strike(ns, forward, expiry)
        option_call = Option(
            asset=market.asset,
            strike=strike,
            expiry=expiry,
            option_type=OptionType.CALL,
            lot_size=1
        )
        option_put = Option(
            asset=market.asset,
            strike=strike,
            expiry=expiry,
            option_type=OptionType.PUT,
            lot_size=1
        )
        call_price = price_option(option_call, market)
        call_risk = calculate_risk(option_call, market)
        put_price = price_option(option_put, market)
        put_risk = calculate_risk(option_put, market)

        strikes.append(strike)
        fwd_strikes.append(get_forward_strike(market, strike, expiry))
        ivs.append(market.vol_surface.get_vol_ns(ns, expiry))
        call_prices.append(call_price)
        call_deltas.append(call_risk.delta)
        call_gammas.append(call_risk.gamma)
        call_vegas.append(call_risk.vega / 100) # per 1% vol move
        call_thetas.append(call_risk.theta / 365) # per day
        put_prices.append(put_price)
        put_deltas.append(put_risk.delta)
        put_gammas.append(put_risk.gamma)
        put_vegas.append(put_risk.vega / 100) # per 1% vol move
        put_thetas.append(put_risk.theta / 365) # per day

    df = pd.DataFrame({
        'Call Theta': call_thetas,
        'Call Vega': call_vegas,
        'Call Gamma': call_gammas,
        'Call Delta': call_deltas,
        'Call Price': call_prices,
        'Norm Strike': norm_strikes,
        'Strike': strikes,
        'Forward Strike': fwd_strikes,
        'Implied Vol': ivs,
        'Put Price': put_prices,
        'Put Delta': put_deltas,
        'Put Gamma': put_gammas,
        'Put Vega': put_vegas,
        'Put Theta': put_thetas
    })
    return df