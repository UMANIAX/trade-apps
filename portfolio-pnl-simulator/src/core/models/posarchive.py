from dataclasses import dataclass

from src.core.models.position import Position

@dataclass
class PosArchive:
    position: Position
    price: float
    time: float