from .helper import randomize_color

DEBUG = False
LOG_COLOR_RANDOMIZE = False


def colored(text, fr=None, fg=None, fb=None, br=None, bg=None, bb=None):
  if fr is None:
    return f'\033[0m{text}\033[0m'
  elif br is None:
    if LOG_COLOR_RANDOMIZE:
      fr, fg, fb = randomize_color(fr, fg, fb)
    return f'\033[38;2;{fr};{fg};{fb}m{text}\033[0m'
  else:
    if LOG_COLOR_RANDOMIZE:
      fr, fg, fb = randomize_color(fr, fg, fb)
      br, bg, bb = randomize_color(br, bg, bb)
    return f'\033[38;2;{fr};{fg};{fb};48;2;{br};{bg};{bb}m{text}\033[0m'


def fatal(message: str, rets=None, e: Exception = None):
  print(colored(message, 255, 64, 64))
  if e is not None: print(colored(str(e), 255, 64, 64))
  exit(rets if rets is not None else 1)


def error(message: str, rets=None, e: Exception = None):
  print(colored(message, 255, 128, 64))
  if e is not None: print(colored(str(e), 255, 128, 64))


def warn(message: str, e: Exception = None):
  print(colored(message, 255, 192, 0))
  if e is not None: print(colored(str(e), 255, 192, 0))


def info(message: str):
  print(colored(message, 64, 192, 128))


def debug(message: str):
  if DEBUG: print(colored(message, 128, 192, 255))
