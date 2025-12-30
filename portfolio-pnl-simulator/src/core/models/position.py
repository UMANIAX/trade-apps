from dataclasses import dataclass

from src.core.models.security import Security

@dataclass
class Position:
    security: Security
    quantity: int # Positive for long, negative for short