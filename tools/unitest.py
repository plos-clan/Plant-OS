from enum import Enum
import os

class TokenType(Enum):
        ABSTRACT_OPERATOR = 0
        STRING = 1
        LEXER_PLACEHOLDER = 2
        IN_MACRO = 3
        CHAR = 4
        ABSTRACT_SYMBOL = 5
        ENTER = 6

def lexer(content: list[str]) -> list:
        ret = []
        for i, line in enumerate(content):
                in_macro_flag = False
                in_string_flag = False
                in_char_flag = False

                if content[i-1] != '' and content[i-1][-1] == '\\':
                        ret.pop()
                else:
                        ret.append([TokenType.ENTER])

                for ch in line:
                        if in_macro_flag == True:
                                ret[-1] += ch
                        elif in_string_flag == True:
                                if ch == '"':
                                        if ret[-1][0] == TokenType.STRING and ret[-1][1] == '\\':
                                                ret.append([TokenType.STRING, '"'])
                                        else:
                                                ret.append([TokenType.LEXER_PLACEHOLDER])
                                                in_string_flag = False
                                else:
                                        ret[-1][-1] += ch
                        elif in_char_flag == True:
                                if ch == '\'':
                                        if ret[-1][0] == TokenType.CHAR and ret[-1][1] == '\\':
                                                ret.append([TokenType.CHAR, '\''])
                                        else:
                                                ret.append([TokenType.LEXER_PLACEHOLDER])
                                                in_char_flag = False
                                else:
                                        ret[-1][-1] += ch
                        elif ch == ' ':
                                ret.append([TokenType.LEXER_PLACEHOLDER])
                        elif ch == '~':
                                ret.append([TokenType.ABSTRACT_OPERATOR, '~'])
                        elif ch == '!':
                                ret.append([TokenType.ABSTRACT_OPERATOR, '!'])
                        elif ch == '#':
                                if in_macro_flag == True:
                                        in_macro_flag = False
                                else:
                                        ret.append([TokenType.IN_MACRO, ''])
                                        in_macro_flag = True
                        elif ch == '%':
                                ret.append([TokenType.ABSTRACT_OPERATOR, '%'])
                        elif ch == '^':
                                ret.append([TokenType.ABSTRACT_OPERATOR, '^'])
                        elif ch == '&':
                                if ret[-1][0] == TokenType.ABSTRACT_OPERATOR and ret[-1][1] == '&':
                                        ret[-1][1] += '&'
                                else:
                                        ret.append([TokenType.ABSTRACT_OPERATOR, '&'])
                        elif ch == '*':
                                ret.append([TokenType.ABSTRACT_OPERATOR, '*'])
                        elif ch == '(':
                                ret.append([TokenType.ABSTRACT_OPERATOR, '('])
                        elif ch == ')':
                                ret.append([TokenType.ABSTRACT_OPERATOR, ')'])
                        elif ch == '-':
                                ret.append([TokenType.ABSTRACT_OPERATOR, '-'])
                        elif ch == '=':
                                if ret[-1][0] == TokenType.ABSTRACT_OPERATOR and (
                                        ret[-1][1] == '!'
                                        or ret[-1][1] == '%'
                                        or ret[-1][1] == '^'
                                        or ret[-1][1] == '&'
                                        or ret[-1][1] == '*'
                                        or ret[-1][1] == '-'
                                        or ret[-1][1] == '='
                                        or ret[-1][1] == '+'
                                        or ret[-1][1] == '<'
                                        or ret[-1][1] == '>'
                                        or ret[-1][1] == '/'
                                        or ret[-1][1] == '|'):
                                        ret[-1][1] += '='
                                else:
                                        ret.append([TokenType.ABSTRACT_OPERATOR, '='])
                        elif ch == '[':
                                ret.append([TokenType.ABSTRACT_OPERATOR, '['])
                        elif ch == ']':
                                ret.append([TokenType.ABSTRACT_OPERATOR, ']'])
                        elif ch == '{':
                                ret.append([TokenType.ABSTRACT_OPERATOR, '{'])
                        elif ch == '}':
                                ret.append([TokenType.ABSTRACT_OPERATOR, '}'])
                        elif ch == '|':
                                if ret[-1][0] == TokenType.ABSTRACT_OPERATOR and ret[-1][1] == '|':
                                        ret[-1][1] += '|'
                                else:
                                        ret.append([TokenType.ABSTRACT_OPERATOR, '|'])
                        elif ch == ':':
                                ret.append([TokenType.ABSTRACT_OPERATOR, ':'])
                        elif ch == ';':
                                ret.append([TokenType.ABSTRACT_OPERATOR, ';'])
                        elif ch == ',':
                                ret.append([TokenType.ABSTRACT_OPERATOR, ','])
                        elif ch == '.':
                                ret.append([TokenType.ABSTRACT_OPERATOR, '.'])
                        elif ch == '?':
                                ret.append([TokenType.ABSTRACT_OPERATOR, '?'])
                        elif ch == '/':
                                ret.append([TokenType.ABSTRACT_OPERATOR, '/'])
                        elif ch == '<':
                                if ret[-1][0] == TokenType.ABSTRACT_OPERATOR and ret[-1][1] == '<':
                                        ret[-1][1] += '<'
                                else:
                                        ret.append([TokenType.ABSTRACT_OPERATOR, '<'])
                        elif ch == '>':
                                if ret[-1][0] == TokenType.ABSTRACT_OPERATOR and ret[-1][1] == '>':
                                        ret[-1][1] += '>'
                                else:
                                        ret.append([TokenType.ABSTRACT_OPERATOR, '>'])
                        elif ch == '"':
                                ret.append([TokenType.STRING, ''])
                                in_string_flag = True
                        elif ch == '\'':
                                ret.append([TokenType.CHAR, ''])
                                in_char_flag
                        else:
                                if ret[-1][0] == TokenType.ABSTRACT_SYMBOL:
                                        ret[-1][1] += ch
                                else:
                                        ret.append([TokenType.ABSTRACT_SYMBOL, ch])

        # 注释掉这段可以省略一个 pass ，本程序不用这段代码，但是这个程序的修改版可能要使用这段代码
        # bakret = ret.copy()
        # offset = 0
        # for i, list_ in enumerate(bakret):
        #         if list_[0] == ElementType.LEXER_PLACEHOLDER:
        #                 ret.pop(i - offset)
        #                 offset += 1

        return ret

def build_code_from_tokens(tokens: list) -> str:
        identlevel = 1  # 少一个 pass ，不用再额外加 ident
        ret = ""
        for i, token in enumerate(tokens):
                if token[0] == TokenType.ENTER:
                        ret += '\n' + identlevel * "    "
                elif token[0] == TokenType.ABSTRACT_OPERATOR and token[1] == '{':
                        identlevel += 1
                        ret += '{'
                elif token[0] == TokenType.ABSTRACT_OPERATOR and token[1] == '(':
                        ret += '('
                elif token[0] == TokenType.ABSTRACT_OPERATOR and token[1] == ')':
                        ret = ret[:-1]
                        ret += ')'
                elif token[0] == TokenType.ABSTRACT_OPERATOR and token[1] == '}':
                        identlevel -= 1
                        ret = ret[:-4]
                        ret += '} '
                elif token[0] == TokenType.ABSTRACT_OPERATOR and token[1] == ';':
                        ret = ret[:-1]
                        ret += token[1]
                elif token[0] != TokenType.LEXER_PLACEHOLDER:
                        ret += token[1] + " "
        return ret

def catch_unitests(filename) -> list[str]:
        with open(filename,'r') as file:
                content = file.read()
        tokens = lexer(content.splitlines())
        unitest_flags = []
        braces_stack = 0
        unitest_started_flag = False
        for i, token in enumerate(tokens):
                if token[0] == TokenType.ABSTRACT_SYMBOL and token[1] == 'unitest':
                        unitest_flags.append([i + 3])
                        unitest_started_flag = True
                elif unitest_started_flag:
                        if token[0] == TokenType.ABSTRACT_OPERATOR and token[1] == '{':
                                braces_stack += 1
                        elif token[0] == TokenType.ABSTRACT_OPERATOR and token[1] == '}':
                                braces_stack -= 1
                                if braces_stack == 0:
                                        unitest_started_flag = False
                                        unitest_flags[-1].append(i - 1)
        ret = []
        for i, (start, end) in enumerate(unitest_flags):
                code = build_code_from_tokens(tokens[start: end])
                if not os.path.exists('build/unitests/' + os.path.dirname(filename)):
                        os.makedirs('build/unitests/' + os.path.dirname(filename))
                outputfilename = ('build/unitests/'
                        + os.path.splitext(filename)[0]
                        + '-' + str(i)
                        + os.path.splitext(filename)[1])
                ret.append(outputfilename)
                with open(outputfilename, 'w') as file:
                        file.write("\
#include<" + filename + '''>
int main(void) {''' + code + '''
}''')
        return ret

def scan_files(dirname) -> str:
        ret = []
        for root, dirs, files in os.walk(dirname):
                for file in files:
                        ret.append(root + '/' + file)
        return ret

def build_all_files() -> list[str]:
        ret = []
        for file in scan_files('src'):
                fileext = os.path.splitext(file)[1]
                if (fileext == '.c'
                or fileext == '.cpp'):
                        ret.extend(catch_unitests(file))
        return ret
