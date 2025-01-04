import math
from copy import copy, deepcopy
from .logging import *

_SAFE_GLOBALS = [{
    '__builtins__': {
        func.__name__: func
        for func in {
            print,
            abs,
            min,
            max,
            len,
            sum,
            sorted,
            range,
            enumerate,
            filter,
            map,
            zip,
            all,
            any,
            chr,
            ord,
            divmod,
            pow,
            round,
            isinstance,
            issubclass,
            type,
            id,
            repr,
            str,
            int,
            float,
            bool,
            complex,
            bytes,
            bytearray,
            memoryview,
            tuple,
            list,
            set,
            frozenset,
            dict,
            object,
            format,
            bin,
            oct,
            hex,
            hash,
            callable,
            vars,
            locals,
            globals,
            next,
            iter,
            reversed,
            slice,
            staticmethod,
            classmethod,
            property,
        }
    },
    'True': True,
    'False': False,
}, {
    debug,
    info,
    warn,
    error,
    fatal,
}, {
    math.sin,
    math.cos,
    math.tan,
}]

SAFE_GLOBALS = {}
for globals in _SAFE_GLOBALS:
  if isinstance(globals, set):
    SAFE_GLOBALS.update({func.__name__: func for func in globals})
  elif isinstance(globals, list):
    SAFE_GLOBALS.update({func.__name__: func for func in globals})
  elif isinstance(globals, dict):
    SAFE_GLOBALS.update(globals)
  else:
    fatal('Invalid safe globals')
del _SAFE_GLOBALS
