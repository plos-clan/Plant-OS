from pygments.lexer import RegexLexer
from pygments.token import Keyword, Name, Operator


class FilesLexer(RegexLexer):
  name = 'Files'
  aliases = ['files']
  filenames = []

  tokens = {
      'root': [
          (r'.+/$', Operator),
          (r'.+\.lm$', Name.Attribute),
          (r'.+\.lh$', Keyword),
          (r'.+\.yaml$', Name.Class),
          (r'.+$', Name.Function),
      ]
  }
