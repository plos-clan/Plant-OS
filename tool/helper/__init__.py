from .color import *
from .path import *


def flatten(lst: list) -> list:
  return [item for sublist in lst for item in sublist]
