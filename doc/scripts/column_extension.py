from markdown.extensions import Extension
from markdown.preprocessors import Preprocessor
from markdown.inlinepatterns import InlineProcessor
from xml.etree import ElementTree as etree
import re
'''

markdown 纵向排列扩展。语法如下，不可嵌套。

:::column

这是第一列的内容。

:::

这是第二列的内容。

:::

这是第三列的内容。

:::endcolumn

也可以使用行内版本：

:: 这是第一列的内容 :: 这是第二列的内容 :: 这是第三列的内容 ::

'''


class ColumnPreprocessor(Preprocessor):
  COLUMN_RE = re.compile(r'^:::column\n(.*?)\n:::endcolumn$', re.DOTALL | re.MULTILINE)

  def run(self, lines):
    text = '\n'.join(lines)
    while True:
      match = self.COLUMN_RE.search(text)
      if not match: break
      content = ('\n' + match.group(1)).split('\n:::')
      replacement = []
      for c in content:
        if c.strip() == '': continue
        b, c = c.split('\n', maxsplit=1)
        replacement.append((f'<div class="{b}">' if b.strip() else '<div>') + self.md.convert(c) + '</div>')
      replacement = ''.join(replacement)
      text = text[:match.start()] + f'<div class="column">{replacement}</div>' + text[match.end():]
    return text.split('\n')


class ColumnInlineProcessor(InlineProcessor):

  def __init__(self):
    super().__init__(r'^\s*::(.*?::.*?)::\s*$')

  def handleMatch(self, m, data):
    el = etree.Element('div')
    el.set('class', 'column')
    for content in m.group(1).split('::'):
      column = etree.Element('div')
      column.text = content.strip()
      el.append(column)
    return el, m.start(0), m.end(0)


class ColumnExtension(Extension):

  def extendMarkdown(self, md):
    md.preprocessors.register(ColumnPreprocessor(md), 'columnblock', 175)
    md.inlinePatterns.register(ColumnInlineProcessor(), 'columncontent', 175)


def makeExtension(**kwargs):
  return ColumnExtension(**kwargs)
