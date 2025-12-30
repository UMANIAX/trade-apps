from dataclasses import dataclass

from src.core.models.regime import Regime
from src.core.models.asset import Asset
from src.core.models.volsurface import VolSurface

@dataclass
class Market:
    asset: Asset
    time: int
    spot: float
    vol_surface: VolSurface
    interest_rate: float
    regime: Regime
