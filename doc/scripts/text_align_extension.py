from markdown.extensions import Extension
from markdown.inlinepatterns import InlineProcessor
from xml.etree import ElementTree as etree


class TextAlignLeftPreprocessor(InlineProcessor):

  def __init__(self):
    super().__init__(r'^\s*::(([^:]|:[^:])*?)\s*$')

  def handleMatch(self, m, data):
    el = etree.Element('span')
    el.text = m.group(1).strip()
    el.set('style', 'display: inline-block; width: 100%; text-align: left')
    return el, m.start(0), m.end(0)


class TextAlignRightPreprocessor(InlineProcessor):

  def __init__(self):
    super().__init__(r'^\s*(([^:]|:[^:])*?)::\s*$')

  def handleMatch(self, m, data):
    el = etree.Element('span')
    el.text = m.group(1).strip()
    el.set('style', 'display: inline-block; width: 100%; text-align: right')
    return el, m.start(0), m.end(0)


class TextAlignCenterPreprocessor(InlineProcessor):

  def __init__(self):
    super().__init__(r'^\s*::(([^:]|:[^:])*?)::\s*$')

  def handleMatch(self, m, data):
    el = etree.Element('span')
    el.text = m.group(1).strip()
    el.set('style', 'display: inline-block; width: 100%; text-align: center')
    return el, m.start(0), m.end(0)


class TextAlignExtension(Extension):

  def extendMarkdown(self, md):
    md.inlinePatterns.register(TextAlignLeftPreprocessor(), 'text_align_left', 175)
    md.inlinePatterns.register(TextAlignRightPreprocessor(), 'text_align_right', 175)
    md.inlinePatterns.register(TextAlignCenterPreprocessor(), 'text_align_center', 175)


def makeExtension(**kwargs):
  return TextAlignExtension(**kwargs)
