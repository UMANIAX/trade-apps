from dataclasses import dataclass

from src.core.models.security import Security
from src.core.models.asset import Asset

@dataclass
class Future(Security):
    asset: Asset
    expiry: str
    lot_size: int