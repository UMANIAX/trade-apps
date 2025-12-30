from dataclasses import dataclass

@dataclass
class Risk:
    delta: float
    gamma: float
    vega: float
    theta: float