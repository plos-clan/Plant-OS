import os
from ..helper import *


class CFileGenerator:

  def __init__(self, name: str, workdir: str = None, file=None):
    if file is not None:
      self.name = name
      self.workdir = workdir
      self.file = file
      return
    workdir = given_path_or_workdir(workdir)
    self.name = name
    self.workdir = workdir
    self.file = None

  def __enter__(self):
    self.file = open(os.path.join(self.workdir, self.name), 'w')
    self.flag('generated')
    return self

  def __exit__(self, exc_type, exc_value, traceback):
    self.file.close()
    self.file = None

  def line(self, line: str):
    self.file.write(f'{line}\n')

  def comment(self, text: str):
    self.file.writelines([f'// {l}' for l in text.splitlines()])

  def once(self):
    self.file.write(f'#pragma once\n')

  def flag(self, flag: str):
    self.file.write(f'// %% {flag} %%\n')

  def ifdef(self, cond: str):

    class IfDefBlock(CFileGenerator):

      def __init__(self, parent: CFileGenerator, cond: str):
        super().__init__(parent.name, parent.workdir, parent.file)
        self.cond = cond

      def __enter__(self):
        self.file.write(f'#ifdef {self.cond}\n')

      def __exit__(self, exc_type, exc_value, traceback):
        self.file.write(f'#endif // {self.cond}\n')

    return IfDefBlock(self, cond)

  def ifndef(self, cond: str):

    class IfNDefBlock(CFileGenerator):

      def __init__(self, parent: CFileGenerator, cond: str):
        super().__init__(parent.name, parent.workdir, parent.file)
        self.cond = cond

      def __enter__(self):
        self.file.write(f'#ifndef {self.cond}\n')

      def __exit__(self, exc_type, exc_value, traceback):
        self.file.write(f'#endif // {self.cond}\n')

    return IfNDefBlock(self, cond)

  def if_cpp(self):
    return self.ifdef('__cplusplus')

  def extern_c(self):

    class ExternCBlock(CFileGenerator):

      def __init__(self, parent: CFileGenerator):
        super().__init__(parent.name, parent.workdir, parent.file)

      def __enter__(self):
        self.file.write('extern "C" {\n')

      def __exit__(self, exc_type, exc_value, traceback):
        self.file.write('} // extern "C"\n')

    return ExternCBlock(self)

  def extern_cpp(self):

    class ExternCPPBlock(CFileGenerator):

      def __init__(self, parent: CFileGenerator):
        super().__init__(parent.name, parent.workdir, parent.file)

      def __enter__(self):
        self.file.write('extern "C++" {\n')

      def __exit__(self, exc_type, exc_value, traceback):
        self.file.write('} // extern "C++"\n')

    return ExternCPPBlock(self)

  def include(self, header: str, system: bool = False):
    if system:
      self.file.write(f'#include <{header}>\n')
    else:
      self.file.write(f'#include "{header}"\n')

  def define(self, macro: str, value: str = None):
    if value is None: value = ''
    value = value.splitlines()
    if len(value) == 0:
      self.file.write(f'#define {macro}\n')
      return
    if len(value) == 1:
      self.file.write(f'#define {macro} {value[0]}\n')
      return
    self.file.write(f'#define {macro} \\\n')
    for v in value[:-1]:
      self.file.write(f'    {v} \\\n')
    self.file.write(f'    {value[-1]}\n')
