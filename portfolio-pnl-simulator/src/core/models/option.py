from dataclasses import dataclass

from src.core.models.asset import Asset
from src.core.models.optiontype import OptionType
from src.core.models.security import Security

@dataclass
class Option(Security):
    asset: Asset
    strike: float
    expiry: int
    option_type: OptionType
    lot_size: int