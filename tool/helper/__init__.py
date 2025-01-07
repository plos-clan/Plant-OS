import os


def given_path_or_workdir(path: str | None) -> str:
  if path is None: path = os.getcwd()
  if not isinstance(path, str): raise TypeError(f"Invalid type for 'path': {type(path)}")
  if len(path) == 0: path = os.getcwd()
  path = os.path.abspath(path)
  if not os.path.isdir(path): raise FileNotFoundError(f"Directory '{path}' not found.")
  return path


def given_path_or_default(path: str | None, default: str, isdir: bool = False) -> str:
  if not isinstance(default, str): raise TypeError(f"Invalid type for 'default': {type(default)}")
  if len(default) == 0: raise ValueError("Invalid value for 'default': empty string.")
  if path is None: path = default
  if not isinstance(path, str): raise TypeError(f"Invalid type for 'path': {type(path)}")
  if len(path) == 0: path = default
  path = os.path.abspath(path)
  if isdir and not os.path.isdir(path): raise FileNotFoundError(f"Directory '{path}' not found.")
  return path


def flatten(lst: list) -> list:
  return [item for sublist in lst for item in sublist]


def get_files_with_extensions(path: str | list[str], extensions: str | list[str], recurse: bool = False):
  if isinstance(path, list):
    for path in path:
      file_list += get_files_with_extensions(path, extensions)
    return file_list
  if isinstance(extensions, str): extensions = [extensions]
  if not recurse: return [os.path.join(path, f) for f in os.listdir(path) if any([f.endswith(ext) for ext in extensions])]
  file_list = []
  for root, dirs, files in os.walk(path):
    file_list += [os.path.join(root, f) for f in files if any([f.endswith(ext) for ext in extensions])]
  return file_list
