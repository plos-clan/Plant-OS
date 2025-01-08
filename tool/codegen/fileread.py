import os
from ..helper import *


class CCommentReader:

  def __init__(self, filename: str, workdir: str = None):
    workdir = given_path_or_workdir(workdir)
    self.file = open(os.path.join(workdir, filename), 'r', encoding='utf-8')
    self.in_block_comment = False

  def __iter__(self):
    return self

  def __next__(self):
    while True:
      line = self.file.readline()
      if not line: raise StopIteration

      if self.in_block_comment:
        bce = line.find('*/')
        if bce < 0: return line.strip()
        self.in_block_comment = False
        return line[:bce].strip()

      lcs, bcs = line.find('//'), line.find('/*')  # Line comment start, Block comment start

      if lcs != -1 and (bcs == -1 or lcs < bcs):
        return line[lcs + 2:].strip()

      if bcs != -1:
        bce = line.find('*/', bcs + 2)
        if bce != -1: return line[bcs + 2:bce].strip()
        self.in_block_comment = True
        return line[bcs + 2:].strip()


# 读取自定义的 flags 如：
# |   %% flag %%
class CFileFlagReader:

  def __init__(self, filename: str, workdir: str = None):
    self.reader = CCommentReader(filename, workdir)

  def __iter__(self):
    return self

  def __next__(self):
    while True:
      line = next(self.reader)
      if not line: raise StopIteration
      if line.startswith('%%') and line.endswith('%%'):
        return line[2:-2].strip()


def c_file_has_flag(flag: str, filename: str, workdir: str = None) -> bool:
  for f in CFileFlagReader(filename, workdir):
    if f == flag: return True
  return False
