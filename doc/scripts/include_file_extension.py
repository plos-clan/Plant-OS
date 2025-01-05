import os
from markdown.extensions import Extension
from markdown.preprocessors import Preprocessor
import re
'''
匹配 ```lang: filepath``` 的代码块，将 filepath 文件的内容插入到代码块中。
如果文件路径不存在，则输出错误信息。
注意 lang 只能是字母和数字，不能包含空格，lang 前也不能有空格。
filepath 前后的空格是可选的且会被忽略。
'''


class IncludeFilePreprocessor(Preprocessor):
  RE = re.compile(r'```(?P<lang>[A-Za-z0-9]+):\s*(?P<filepath>[^\s]+)\s*```')

  def __init__(self, md, base_path=None):
    super().__init__(md)
    self.base_path = [base_path] if isinstance(base_path, str) else base_path

  def run(self, lines):
    new_lines = []
    for line in lines:
      match = self.RE.match(line)
      if match:
        # 获取语言和文件路径
        lang = match.group('lang')
        file_path = match.group('filepath')

        # 尝试使用当前 Markdown 文件目录
        markdown_file_dir = os.path.dirname(self.md.Meta.get('current_file', ''))
        full_path = os.path.join(markdown_file_dir, file_path)

        file_content = None
        if os.path.exists(full_path):
          file_content = self.read_file(full_path)
        else:  # 尝试使用 base_path
          for base_path in self.base_path:
            full_path = os.path.join(base_path, file_path)
            if os.path.exists(full_path):
              file_content = self.read_file(full_path)
              break
        if file_content is None:
          lang = 'log'
          file_content = f'Error: File not found - {file_path}'

        # 添加文件内容为指定语言的代码块
        new_lines.append(f'```{lang}')
        new_lines += file_content.split('\n')
        new_lines.append('```')
      else:
        new_lines.append(line)
    return new_lines

  def read_file(self, path):
    try:
      with open(path, 'r', encoding='utf-8') as f:
        return f.read()
    except Exception as e:
      return f'Error reading file {path}: {str(e)}'


class IncludeFileExtension(Extension):

  def __init__(self, **kwargs):
    self.config = {'base_path': ['.', 'Base directory for including files']}
    super().__init__(**kwargs)

  def extendMarkdown(self, md):
    base_path = self.getConfig('base_path')
    md.preprocessors.register(IncludeFilePreprocessor(md, base_path), 'include_file', 25)


def makeExtension(**kwargs):
  return IncludeFileExtension(**kwargs)
