import numpy as np
from scipy.stats import norm

from src.core.utils import get_forward_price
from src.core.models.future import Future
from src.core.models.market import Market
from src.core.models.option import Option
from src.core.models.optiontype import OptionType
from src.core.models.risk import Risk
from src.core.models.security import Security

def get_forward_strike(market: Market, strike: float, expiry: int) -> float:
    tte = expiry / 365
    return strike * np.exp(market.interest_rate * tte)

def price_future(future: Future, market: Market) -> float:
    tte = future.expiry / 365
    return get_forward_price(market.spot, market.interest_rate, future.expiry)

def get_option_pricer_inputs(option: Option, market: Market):
    tte = option.expiry / 365
    forward = market.spot * np.exp(market.interest_rate * tte)
    sigma = market.vol_surface.get_vol(option.strike, forward, option.expiry)
    d1 = (np.log(forward / option.strike) + 0.5 * sigma**2 * tte) / (sigma * np.sqrt(tte))
    d2 = d1 - sigma * np.sqrt(tte)
    df = np.exp(-market.interest_rate * tte)
    return tte, forward, sigma, d1, d2, df

def price_option(option: Option, market: Market) -> float:
    tte, forward, _, d1, d2, _ = get_option_pricer_inputs(option, market)
    if option.option_type == OptionType.CALL:
        price = np.exp(-market.interest_rate * tte) * (forward * norm.cdf(d1) - option.strike * norm.cdf(d2))
    else:
        price = np.exp(-market.interest_rate * tte) * (option.strike * norm.cdf(-d2) - forward * norm.cdf(-d1))
    return price

def calculate_risk(option: Option, market: Market) -> Risk:
    tte, forward, sigma, d1, _, df = get_option_pricer_inputs(option, market)
    delta = df * (norm.cdf(d1) if option.option_type == OptionType.CALL else -norm.cdf(-d1))
    gamma = df * norm.pdf(d1) / (forward * sigma * np.sqrt(tte))
    vega = df * forward * norm.pdf(d1) * np.sqrt(tte)
    theta_time = -df * forward * norm.pdf(d1) * sigma / (2.0 * np.sqrt(tte))
    if option.option_type == OptionType.CALL:
        theta_carry = -market.interest_rate * df * forward * norm.cdf(d1)
    else:
        theta_carry = +market.interest_rate * df * forward * norm.cdf(-d1)
    theta = theta_time + theta_carry
    return Risk(delta=delta, gamma=gamma, vega=vega, theta=theta)

def calculate_portfolio_risk(positions: list[Security], market: Market) -> Risk:
    total_delta = 0.0
    total_gamma = 0.0
    total_vega = 0.0
    total_theta = 0.0
    for position in positions:
        if isinstance(position, Option):
            risk = calculate_risk(position, market)
            total_delta += risk.delta * position.lot_size
            total_gamma += risk.gamma * position.lot_size
            total_vega += risk.vega * position.lot_size
            total_theta += risk.theta * position.lot_size
        elif isinstance(position, Future):
            total_delta += 1.0 * position.lot_size
    return Risk(delta=total_delta, gamma=total_gamma, vega=total_vega, theta=total_theta)