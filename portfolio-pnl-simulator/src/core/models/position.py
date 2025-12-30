from dataclasses import dataclass

from src.core.models.security import Security
from src.core.models.txnside import TxnSide

@dataclass
class Position:
    security: Security
    quantity: int # Positive for long, negative for short