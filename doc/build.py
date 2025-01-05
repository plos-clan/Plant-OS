#!/usr/bin/python3
from scripts.lumos import LumosLexer
from scripts.files import FilesLexer


def get_lexer_by_name(_alias, **options):
  if _alias == 'lumos': return LumosLexer(**options)
  if _alias == 'files': return FilesLexer(**options)
  return real_get_lexer_by_name(_alias, **options)


from pygments import lexers

real_get_lexer_by_name = lexers.get_lexer_by_name
lexers.get_lexer_by_name = get_lexer_by_name

from mkdocs import __main__ as mkdocs

mkdocs.cli()
