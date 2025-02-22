import shutil
import os
from .logging import *

if 'CC' not in os.environ:
  warn('CC is not set, try using cc as default')
  cc_path = shutil.which('cc')
  if cc_path is None:
    warn('cc is not found, try using clang or gcc as default')
    cc_path = shutil.which('clang')
    if cc_path is None:
      cc_path = shutil.which('gcc')
      if cc_path is None:
        fatal('cc is not found')
  os.environ['CC'] = cc_path
if 'CXX' not in os.environ:
  warn('CXX is not set, try using c++ as default')
  cxx_path = shutil.which('c++')
  if cxx_path is None:
    warn('c++ is not found, try using clang++ or g++ as default')
    cxx_path = shutil.which('clang++')
    if cxx_path is None:
      cxx_path = shutil.which('g++')
      if cxx_path is None:
        fatal('c++ is not found')
  os.environ['CXX'] = cxx_path

CC = os.environ['CC']
CXX = os.environ['CXX']
