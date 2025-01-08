import os
from ..helper import *
from .filegen import CFileGenerator
from .fileread import CCommentReader, c_file_has_flag


class CCompileFlags:
  include_files = []  # -include xxx.h
  include_dirs = []
  optim = ''  # -O1
  defines = {}  # -Dname=value
  flags = []

  def include(self, file: str | list[str]):
    if isinstance(file, list):
      self.include_files.extend(file)
    else:
      self.include_files.append(file)

  def define(self, name: str | list[str], value: str = None):
    if isinstance(name, list):
      for n in name:
        self.define(n, value)
    else:
      self.defines[name] = value

  def __lshift__(self, other: str | list[str]):
    if isinstance(other, list):
      for flag in other:
        self << flag
      return self

    if not isinstance(other, str): raise TypeError("Unsupported type for << operation")

    if other.startswith('-O'): self.optim = other[2:]
    if other.startswith('-I'): self.include_dirs.append(other[2:])
    if other.startswith('-D'): self.define(*other[2:].split('=', 1))
    else: self.flags.append(other)

    return self

  def __repr__(self):
    cmd = []

    if self.optim:
      if self.optim not in ['0', '1', '2', '3', 'fast', 'g', 's', 'z']: raise ValueError(f"Invalid value for 'optim': {self.optim}")
      cmd.append(f'-O{self.optim}')

    for name, value in self.defines.items():
      if value is not None:
        cmd.append(f'-D{name}={value}')
      else:
        cmd.append(f'-D{name}')

    for dir in self.include_dirs:
      cmd.append(f'-I{dir}')

    for file in self.include_files:
      cmd.append(f'-include {file}')

    return ' '.join(cmd + self.flags)


def mkheader(name: str, srcdir: str = None, workdir: str = None, recurse: bool = False) -> int:
  if name == '.' or name == '..': raise ValueError("Invalid value for 'name': it should not be '.' or '..'.")
  if '/' in name: raise ValueError("Invalid value for 'name': it should not contain slashes.")
  workdir = given_path_or_workdir(workdir)
  srcdir = given_path_or_default(srcdir, os.path.join(workdir, name), isdir=True)

  with CFileGenerator(f'{name}.h', workdir) as w:
    w.flag('include-generated')
    w.once()

    h_files = get_files_with_extensions(srcdir, 'h', recurse)
    hpp_files = get_files_with_extensions(srcdir, 'hpp', recurse)

    if recurse:
      h_files = [f for f in h_files if not c_file_has_flag('include-generated', f)]
      hpp_files = [f for f in hpp_files if not c_file_has_flag('include-generated', f)]

    if len(h_files) > 0:
      with w.if_cpp():
        w.line('extern "C" {')
      for header in h_files:
        w.include(os.path.relpath(header, workdir))
      with w.if_cpp():
        w.line('}')

    if len(hpp_files) > 0:
      with w.if_cpp():
        for header in hpp_files:
          w.include(os.path.relpath(header, workdir))
