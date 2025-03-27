import re
import os
import fnmatch
from .color import *
from .path import *


def flatten(lst: list[list]) -> list:
  return [item for sublist in lst for item in sublist]


def parse_target(target: str) -> list[str] | None:
  if re.match(r'^[a-zA-Z0-9_\-]+$', target) is None: return None
  target = target.split('-')
  return None if len(target) != 3 else target


def scan_files(dirname: str) -> str:
  ret = []
  for root, dirs, files in os.walk(dirname):
    for file in files:
      ret.append(f'{root}/{file}')
  return ret


def match_files(pattern: str, base: str) -> str:
  matches = []
  for root, dirs, files in os.walk(base):
    for filename in fnmatch.filter(files, pattern):
      matches.append(os.path.join(root, filename))
  return matches
