from pygments.lexer import RegexLexer, words, bygroups
from pygments.token import Keyword, Name, String, Number, Punctuation, Operator, Comment, Whitespace


def kwds(lists):
  if isinstance(lists[0], list):
    lists = [r for l in lists for r in l]
  return words(lists, prefix=r'\b', suffix=r'\b')


def chars(s):
  return words([c for c in s])


class AsmLexer(RegexLexer):
  name = 'Asm'
  aliases = ['asm']
  filenames = ['*.asm', '*.s']

  tokens = {
      'root': [
          (r'(\s|\n|\r)+', Whitespace),
          (r';.*?(\n|$)', Comment.Single),
          (r'#[^\n]*', Comment.Preproc),
          (r'".*?"', String),
          (r'\'.*?\'', String.Char),
          (r'\b[0-9]+\b', Number.Integer),
          (r'\b0x[0-9a-fA-F]+\b', Number.Hex),
          (r'\b0b[01]+\b', Number.Bin),
          (r'\b0o[0-7]+\b', Number.Oct),
          (r'\b(?:mov|add|sub|mul|div|jmp|cmp|jne|je|jg|jl|jge|jle|call|ret|push|pop|nop)\b', Keyword),
          (r'\b(?:eax|ebx|ecx|edx|esi|edi|esp|ebp|ax|bx|cx|dx|si|di|sp|bp|al|bl|cl|dl|ah|bh|ch|dh)\b', Name.Builtin),
          (r'[a-zA-Z_][a-zA-Z0-9_]*:', Name.Label),
          (r'[a-zA-Z_][a-zA-Z0-9_]*', Name),
          (chars('([{'), Punctuation, 'root'),
          (chars(')]}'), Punctuation, '#pop'),
          (chars(',;'), Punctuation),
          (chars('+-*/%=^&|?:<>!~.'), Operator),
      ]
  }
