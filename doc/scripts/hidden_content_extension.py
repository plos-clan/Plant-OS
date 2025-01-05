from markdown.extensions import Extension
from markdown.preprocessors import Preprocessor
from markdown.inlinepatterns import InlineProcessor
from xml.etree import ElementTree as etree
import re
'''

markdown 隐藏内容扩展。语法如下：

????
这是隐藏的内容。
????

也可以使用行内版本：

?? 这是隐藏的内容 ??

'''


class HiddenBlockPreprocessor(Preprocessor):
  HIDDEN_BLOCK_RE = re.compile(r'^\?\?\?\?\n(.*?)\n\?\?\?\?$', re.DOTALL | re.MULTILINE)

  def run(self, lines):
    text = '\n'.join(lines)
    while True:
      match = self.HIDDEN_BLOCK_RE.search(text)
      if not match: break
      content = match.group(1)
      replacement = f'<div class="hidden-content">{self.md.convert(content)}</div>'
      text = text[:match.start()] + replacement + text[match.end():]
    return text.split('\n')


class HiddenContentInlineProcessor(InlineProcessor):

  def __init__(self):
    super().__init__(r'\?\? *(..*?) *\?\?')

  def handleMatch(self, m, data):
    el = etree.Element('span')
    el.set('class', 'hidden-content')
    el.text = m.group(1).strip()
    return el, m.start(0), m.end(0)


class HiddenContentExtension(Extension):

  def extendMarkdown(self, md):
    md.preprocessors.register(HiddenBlockPreprocessor(md), 'hiddenblock', 175)
    md.inlinePatterns.register(HiddenContentInlineProcessor(), 'hiddencontent', 175)


def makeExtension(**kwargs):
  return HiddenContentExtension(**kwargs)
