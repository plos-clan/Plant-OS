#!/bin/env python
from enum       import Enum
from typing     import Any
import os
import curses
import unicodedata  # Some Unix-like OS only
from io import StringIO
import subprocess
import ast

tool_name_alias_list = {
        "ttts": "transform_tab_to_spaces",
        "make": "../make"
}

class TextAttribute(Enum):
        ALTCHARSET = curses.A_ALTCHARSET
        ATTRIBUTES = curses.A_ATTRIBUTES
        BLINK      = curses.A_BLINK
        BOLD       = curses.A_BOLD
        CHARTEXT   = curses.A_CHARTEXT
        COLOR      = curses.A_COLOR
        DIM        = curses.A_DIM
        HORIZONTAL = curses.A_HORIZONTAL
        INVIS      = curses.A_INVIS
        ITALIC     = curses.A_ITALIC
        LEFT       = curses.A_LEFT
        LOW        = curses.A_LOW
        NORMAL     = curses.A_NORMAL
        PROTECT    = curses.A_PROTECT
        REVERSE    = curses.A_REVERSE
        RIGHT      = curses.A_RIGHT
        STANDOUT   = curses.A_STANDOUT
        TOP        = curses.A_TOP
        UNDERLINE  = curses.A_UNDERLINE
        VERTICAL   = curses.A_VERTICAL

class SpecialKey(Enum):
        A1        = curses.KEY_A1
        A3        = curses.KEY_A3
        B2        = curses.KEY_B2
        BACKSPACE = curses.KEY_BACKSPACE
        BEG       = curses.KEY_BEG
        BREAK     = curses.KEY_BREAK
        BTAB      = curses.KEY_BTAB
        C1        = curses.KEY_C1
        C3        = curses.KEY_C3
        CANCEL    = curses.KEY_CANCEL
        CATAB     = curses.KEY_CATAB
        CLEAR     = curses.KEY_CLEAR
        CLOSE     = curses.KEY_CLOSE
        COMMAND   = curses.KEY_COMMAND
        COPY      = curses.KEY_COPY
        CREATE    = curses.KEY_CREATE
        CTAB      = curses.KEY_CTAB
        DC        = curses.KEY_DC
        DL        = curses.KEY_DL
        DOWN      = curses.KEY_DOWN
        EIC       = curses.KEY_EIC
        END       = curses.KEY_END
        ENTER     = curses.KEY_ENTER
        EOL       = curses.KEY_EOL
        EOS       = curses.KEY_EOS
        EXIT      = curses.KEY_EXIT
        F0        = curses.KEY_F0
        F1        = curses.KEY_F1
        F10       = curses.KEY_F10
        F11       = curses.KEY_F11
        F12       = curses.KEY_F12
        F13       = curses.KEY_F13
        F14       = curses.KEY_F14
        F15       = curses.KEY_F15
        F16       = curses.KEY_F16
        F17       = curses.KEY_F17
        F18       = curses.KEY_F18
        F19       = curses.KEY_F19
        F2        = curses.KEY_F2
        F20       = curses.KEY_F20
        F21       = curses.KEY_F21
        F22       = curses.KEY_F22
        F23       = curses.KEY_F23
        F24       = curses.KEY_F24
        F25       = curses.KEY_F25
        F26       = curses.KEY_F26
        F27       = curses.KEY_F27
        F28       = curses.KEY_F28
        F29       = curses.KEY_F29
        F3        = curses.KEY_F3
        F30       = curses.KEY_F30
        F31       = curses.KEY_F31
        F32       = curses.KEY_F32
        F33       = curses.KEY_F33
        F34       = curses.KEY_F34
        F35       = curses.KEY_F35
        F36       = curses.KEY_F36
        F37       = curses.KEY_F37
        F38       = curses.KEY_F38
        F39       = curses.KEY_F39
        F4        = curses.KEY_F4
        F40       = curses.KEY_F40
        F41       = curses.KEY_F41
        F42       = curses.KEY_F42
        F43       = curses.KEY_F43
        F44       = curses.KEY_F44
        F45       = curses.KEY_F45
        F46       = curses.KEY_F46
        F47       = curses.KEY_F47
        F48       = curses.KEY_F48
        F49       = curses.KEY_F49
        F5        = curses.KEY_F5
        F50       = curses.KEY_F50
        F51       = curses.KEY_F51
        F52       = curses.KEY_F52
        F53       = curses.KEY_F53
        F54       = curses.KEY_F54
        F55       = curses.KEY_F55
        F56       = curses.KEY_F56
        F57       = curses.KEY_F57
        F58       = curses.KEY_F58
        F59       = curses.KEY_F59
        F6        = curses.KEY_F6
        F60       = curses.KEY_F60
        F61       = curses.KEY_F61
        F62       = curses.KEY_F62
        F63       = curses.KEY_F63
        F7        = curses.KEY_F7
        F8        = curses.KEY_F8
        F9        = curses.KEY_F9
        FIND      = curses.KEY_FIND
        HELP      = curses.KEY_HELP
        HOME      = curses.KEY_HOME
        IC        = curses.KEY_IC
        IL        = curses.KEY_IL
        LEFT      = curses.KEY_LEFT
        LL        = curses.KEY_LL
        MARK      = curses.KEY_MARK
        MAX       = curses.KEY_MAX
        MESSAGE   = curses.KEY_MESSAGE
        MIN       = curses.KEY_MIN
        MOUSE     = curses.KEY_MOUSE
        MOVE      = curses.KEY_MOVE
        NEXT      = curses.KEY_NEXT
        NPAGE     = curses.KEY_NPAGE
        OPEN      = curses.KEY_OPEN
        OPTIONS   = curses.KEY_OPTIONS
        PPAGE     = curses.KEY_PPAGE
        PREVIOUS  = curses.KEY_PREVIOUS
        PRINT     = curses.KEY_PRINT
        REDO      = curses.KEY_REDO
        REFERENCE = curses.KEY_REFERENCE
        REFRESH   = curses.KEY_REFRESH
        REPLACE   = curses.KEY_REPLACE
        RESET     = curses.KEY_RESET
        RESIZE    = curses.KEY_RESIZE
        RESTART   = curses.KEY_RESTART
        RESUME    = curses.KEY_RESUME
        RIGHT     = curses.KEY_RIGHT
        SAVE      = curses.KEY_SAVE
        SBEG      = curses.KEY_SBEG
        SCANCEL   = curses.KEY_SCANCEL
        SCOMMAND  = curses.KEY_SCOMMAND
        SCOPY     = curses.KEY_SCOPY
        SCREATE   = curses.KEY_SCREATE
        SDC       = curses.KEY_SDC
        SDL       = curses.KEY_SDL
        SELECT    = curses.KEY_SELECT
        SEND      = curses.KEY_SEND
        SEOL      = curses.KEY_SEOL
        SEXIT     = curses.KEY_SEXIT
        SF        = curses.KEY_SF
        SFIND     = curses.KEY_SFIND
        SHELP     = curses.KEY_SHELP
        SHOME     = curses.KEY_SHOME
        SIC       = curses.KEY_SIC
        SLEFT     = curses.KEY_SLEFT
        SMESSAGE  = curses.KEY_SMESSAGE
        SMOVE     = curses.KEY_SMOVE
        SNEXT     = curses.KEY_SNEXT
        SOPTIONS  = curses.KEY_SOPTIONS
        SPREVIOUS = curses.KEY_SPREVIOUS
        SPRINT    = curses.KEY_SPRINT
        SR        = curses.KEY_SR
        SREDO     = curses.KEY_SREDO
        SREPLACE  = curses.KEY_SREPLACE
        SRESET    = curses.KEY_SRESET
        SRIGHT    = curses.KEY_SRIGHT
        SRSUME    = curses.KEY_SRSUME
        SSAVE     = curses.KEY_SSAVE
        SSUSPEND  = curses.KEY_SSUSPEND
        STAB      = curses.KEY_STAB
        SUNDO     = curses.KEY_SUNDO
        SUSPEND   = curses.KEY_SUSPEND
        UNDO      = curses.KEY_UNDO
        UP        = curses.KEY_UP

        def is_special_key(ch):
                '''
                这个函数不限制参数类型
                输出其类型是否是 SpecialKey
                因为 Window.wait_char() 的返回类型可能是 int、str、SpecialKey
                '''
                if type(ch) == SpecialKey:
                        return True
                else:
                        return False

# colorpair 在 Screen.init 中初始化
class Highlight(Enum):
        FUNCTION = 1
        STRING = 2

def get_key_name(key):
        return curses.keyname(key)

def can_change_color() -> bool:
        return curses.can_change_color()


def char_display_wide(ch) -> int:
        '''
        对于一些字符，例如汉字，它们会在终端中占用两格宽度
        这个函数可以获取字符的显示宽度，以正确在终端中排版这些字符
        '''
        if unicodedata.east_asian_width(ch) in ("W", "F"):
                return 2
        else:
                return 1


def str_display_len(str_) -> int:
        len_ = 0
        for ch in str_:
                len_ += char_display_wide(ch)
        return len_


class Window:
        """
        坐标定位和 Curses 不同
        先 X 再 Y ，(0, 0) 为屏幕左下角
        编辑指针是从 0 开始的，指向下一个字符
        """

        __begin_x: int  # 左上角 x
        __begin_y: int  # 左上角 y
        __end_x  : int  # 右下角 x
        __end_y  : int  # 右下角 y

        __content       = ""  # 现在 Window 内的内容
        __display_point = 0  # 显示的编辑指针
        __real_point    = 0  # 可作用于 Content 的编辑指针
        # 有两个指针是为了解决 CJK 字符在终端中显示为两字符宽度的问题

        def add_begin_y(self, add: int):
                self.__begin_y += add
                self.__curses_window.mvwin(
                        Window.__reverse_pos(
                                Screen.get_height(),
                                self.__begin_y
                        ), self.__begin_x)
                self.__curses_window.resize(self.get_height(), self.get_width())

        def set_begin_x(self, begin_x):
                if begin_x < 0:
                        begin_x = Window.__reverse_pos(Screen.get_width(), begin_x)

                self.__begin_x = begin_x
                self.__curses_window.mvwin(
                        Window.__reverse_pos(
                                Screen.get_height(),
                                self.__begin_y
                        ), self.__begin_x)
                self.__curses_window.resize(self.get_height(), self.get_width())

        def set_begin_y(self, begin_y):
                if begin_y < 0:
                        begin_y = Window.__reverse_pos(Screen.get_height(), begin_y)

                self.__begin_y = begin_y
                self.__curses_window.mvwin(
                        Window.__reverse_pos(
                                Screen.get_height(),
                                self.__begin_y
                        ), self.__begin_x)
                self.__curses_window.resize(self.get_height(), self.get_width())

        def set_begin_xy(self, begin_x, begin_y):
                if begin_x < 0:
                        begin_x = Window.__reverse_pos(Screen.get_width(), begin_x)
                if begin_y < 0:
                        begin_y = Window.__reverse_pos(Screen.get_height(), begin_y)

                self.__begin_x = begin_x
                self.__begin_y = begin_y

                self.__curses_window.mvwin(
                        Window.__reverse_pos(
                                Screen.get_height(),
                                self.__begin_y
                        ), self.__begin_x)
                self.__curses_window.resize(self.get_height(), self.get_width())

        def set_end_x(self, end_x):
                if end_x < 0:
                        end_x = Window.__reverse_pos(Screen.get_width(), end_x)

                self.__end_x = end_x
                self.__curses_window.resize(self.get_height(), self.get_width())

        def set_end_y(self, end_y):
                if end_y < 0:
                        end_y = Window.__reverse_pos(Screen.get_height(), end_y)

                self.__end_y = end_y
                self.__curses_window.resize(self.get_height(), self.get_width())

        def set_end_xy(self, end_x, end_y):
                if end_x < 0:
                        end_x = Window.__reverse_pos(Screen.get_width(), end_x)
                if end_y < 0:
                        end_y = Window.__reverse_pos(Screen.get_height(), end_y)

                self.__end_x = end_x
                self.__end_y = end_y
                self.__curses_window.resize(self.get_height(), self.get_width())

        def __reverse_pos(field_len, pos_weight: int):
                '''
                Example:
                        __reverse_pos(self.get_height(), 0)
                        将 Screen 形式的 y 坐标转化为 Curses 形式

                        __reverse_pos(self.get_height(), -1)
                        类似 list_[-1] 的形式使用坐标
                '''
                if pos_weight >= 0:
                        return field_len - 1 - pos_weight
                else:
                        return field_len + pos_weight
        def __init__(
                self,
                begin_x = 0,
                begin_y = os.get_terminal_size().lines - 1,
                end_x   = os.get_terminal_size().columns - 1,
                end_y   = 0,
        ):

                # 支持使用反转坐标模式，例如 y = -1 是从上往下算第 0 列
                if begin_x < 0:
                        begin_x = Window.__reverse_pos(Screen.get_width(), begin_x)
                if begin_y < 0:
                        begin_y = Window.__reverse_pos(Screen.get_height(), begin_y)
                if end_x < 0:
                        end_x = Window.__reverse_pos(Screen.get_width(), end_x)
                if end_y < 0:
                        end_y = Window.__reverse_pos(Screen.get_height(), end_y)

                self.__curses_window = curses.newwin(
                        begin_y - end_y + 1,
                        end_x - begin_x + 1,
                        Window.__reverse_pos(
                                Screen.get_height(),
                                begin_y
                        ),
                        begin_x
                )
                self.__curses_window.keypad(True)
                self.__begin_x = begin_x
                self.__begin_y = begin_y
                self.__end_x   = end_x
                self.__end_y   = end_y

        def _from_curses_window(
                curses_window,
                nlines: int, ncols: int, begin_y: int, begin_x: int
        ):
                window = Window()

                this_class_begin_y = Window.__reverse_pos(
                        Screen.get_height(),
                        begin_y
                )

                this_class_end_y = Window.__reverse_pos(
                        Screen.get_height(),
                        begin_y + nlines - 1
                )

                window.__begin_x       = begin_x
                window.__begin_y       = this_class_begin_y
                window.__end_x         = begin_x + ncols - 1
                window.__end_y         = this_class_end_y
                window.__curses_window = curses_window
                return window

        def put(
                        self,
                        text,
                        x = 0, y = 0,
                        refresh=True,
                        move_cur=True,
                        move_real_point = True):
                if type(text) == list:
                        text = "".join(text)

                if not move_cur:
                        cur_x, cur_y = self.content_len_to_xy(self.__real_point)

                if move_real_point:
                        self.__real_point = len(text)
                self.__curses_window.clear()
                self.__curses_window.addstr(y, x, text)
                self.__content = text

                if not move_cur:
                        self.move_to(cur_x, cur_y, refresh=False)

                if refresh == True:
                        self.__curses_window.refresh()
                else:
                        pass

                return self

        def clear(self):
                self.__curses_window.clear()
                self.__content = ''
                self.__curses_window.refresh()
                self.__real_point = 0
                self.__display_point = 0

        def add(self,
                str_or_can_be_str: str | Any,
                color: Highlight = None,
                refresh=True):
                str_ = str(str_or_can_be_str)
                self.__content       += str_
                self.__display_point += str_display_len(str_)
                self.__real_point    += len(str_)

                if color == None:
                        self.__curses_window.addstr(str_)
                else:
                        self.__curses_window.addstr(str_, curses.color_pair(color.value))
                if refresh:
                        self.refresh()

        def set_point(self, real_point):
                """
                该函数假定光标永远在已写入的范围内
                """
                if real_point < 0:
                        self.__real_point = 0
                else:
                        self.__real_point = real_point

                x, y = self.content_len_to_xy(self.__real_point)

                self.__display_point = self.xy_to_len(x, y)
                self.move_to(x, y)

        def add_point(self, len_):
                """
                该函数假定光标永远在已写入的范围内
                """

                if self.__real_point + len_ < 0:
                        len_ = self.__real_point

                self.__real_point += len_

                x, y = self.content_len_to_xy(self.__real_point)

                self.__display_point = self.xy_to_len(x, y)
                self.move_to(x, y)

        def get_display_point(self):
                return self.__display_point

        def get_point(self):
                return self.__real_point

        def refresh(self):
                self.__curses_window.refresh()
                return self

        def get_width(self):
                return self.__end_x - self.__begin_x

        def get_height(self):
                return self.__begin_y - self.__end_y + 1

        def get_content(self):
                return self.__content

        def get_max_capacity(self):
                return self.get_width() * self.get_height()

        def get_content_chlist(self):
                return list(self.get_content())

        def len_to_xy(self, len_):
                if len_ <= 0:
                        return 0, 0
                else:
                        y = len_ // self.get_width()
                        x = len_ % self.get_width()
                        return x, y

        def content_len_to_xy(self, len_):
                maxwidth = self.get_width()
                nowwidth = 0
                nowlines = 0
                for i, ch in enumerate(self.__content):
                        if i == len_:
                                break
                        if ch == '\n':
                                nowwidth = 0
                                nowlines += 1
                        else:
                                nowwidth += char_display_wide(ch)

                        if nowwidth == maxwidth:
                                nowwidth = 0
                                nowlines += 1
                return (nowwidth, nowlines)

        def xy_to_len(self, x, y):
                if x == 0:
                        len_ = y * self.get_width()
                else:
                        len_ = (y - 1) * self.get_width() + x

                if len_ < 0:
                        return 0
                else:
                        return len_

        def wait_char(self) -> str | int | SpecialKey:
                """
                这会输出一个 UTF-8 编码的字符或一个控制字符
                """
                ch = self.__curses_window.get_wch()

                if type(ch) == str:
                        return ch
                else:
                        # 如果 ch 是 SpecialKey ，返回对应的 SpecialKey 名称
                        # 如果不是，则返回对应的值
                        try:
                                sk = SpecialKey(ch)
                        except ValueError:
                                return ch
                        else:
                                return sk

        def insert(self, index, str_or_can_be_str: str | Any):
                contentlist = list(self.__content)
                contentlist.insert(index, str(str_or_can_be_str))
                self.put("".join(contentlist), 0, 0, move_real_point=False)

        def move_to(self, x, y, refresh=True):
                '''
                仅移动鼠标，而不包括 points
                '''
                self.__curses_window.move(y, x)
                if refresh:
                        self.refresh()


__STDSCR = None


class Screen:
        """控制 Pyeony 视口本身"""

        def init():
                global __STDSCR
                __STDSCR = curses.initscr()
                curses.noecho()         # 底层开启无回显模式，我们会再套一层
                curses.cbreak()         # 同理，我们不需要默认按回车
                curses.start_color()
                __STDSCR.keypad(True)    # 让 curses 处理特殊按键
                __STDSCR.clear()

                curses.init_pair(
                        Highlight.FUNCTION.value,
                        curses.COLOR_GREEN,
                        curses.COLOR_BLACK
                        )

                curses.init_pair(
                        Highlight.STRING.value,
                        curses.COLOR_YELLOW,
                        curses.COLOR_BLACK
                        )
        def exit():
                curses.echo()
                curses.nocbreak()
                __STDSCR.keypad(False)
                curses.endwin()

        def get_width() -> int:
                return __STDSCR.getmaxyx()[1]

        def get_height() -> int:
                return __STDSCR.getmaxyx()[0]

        def get_main_window() -> Window:
                return Window._from_curses_window(
                        __STDSCR, os.get_terminal_size().lines,
                        os.get_terminal_size().columns,
                        0,
                        0
                )

def preprocess(content: str, stringio: StringIO):
        if content == 'alias':
                return str(tool_name_alias_list)
        elif content[0] == '@':
                result = subprocess.run(content[1:].split(' '),
                                        stdout=subprocess.PIPE,
                                        stderr=subprocess.STDOUT)
                if result.stdout != None:
                        stringio.write(result.stdout.decode())
                if result.stderr != None:
                        stringio.write(result.stderr.decode())
                return None
        elif content[0] == '`':
                command_list = content[1:].split(' ')
                if command_list[0] in tool_name_alias_list.keys():
                        command_list[0] = tool_name_alias_list[command_list[0]]
                command_list[0:1] = ["python", 'tools/' + command_list[0] + '.py']
                result = subprocess.run(command_list,
                                        stdout=subprocess.PIPE,
                                        stderr=subprocess.STDOUT)
                if result.stdout != None:
                        # TODO: 驯服 GCC 之道 就在其中（"`make build"在gcc报错的情况下目前会出问题）
                        # 但是单步调试就诡异的没问题了，亦或者和多线程有关
                        stringio.write(result.stdout.decode())
                if result.stderr != None:
                        stringio.write(result.stderr.decode())
                return None
        else:
                return content

Screen.init()

mainwindow = Screen.get_main_window()

inputwindow = Window(
        0, 0,
        -1, 0
)
outputwindow = Window(
        0, -1,
        -1, 1
)

outputwindow.put("""
Pyeony 0.0.2

Press ESC or input \"exit\" to exit
@ls equal to \"ls\" in normal shell
`call equal to \"python tools/call.py\" in normal shell
Input \"alias\" to show tools name alias list\n""")

LOCALS = {} # 它的项将在其它地方被添加

class TokenType(Enum):
        FUNCTION = 0
        STRING = 1

PrecoloredContent = list[tuple[TokenType, str] | str]

def highlight_from_precolored_content(
                window: Window,
                precolored_content: PrecoloredContent):

        for i, item in enumerate(precolored_content):
                if type(item) == tuple:
                        if item[0] == TokenType.STRING:
                                precolored_content[i-1:i+2] = [
                                        (item[0],
                                                  precolored_content[i-1] +
                                                  item[1] +
                                                  precolored_content[i+1])]


        for item in precolored_content:
                if type(item) == tuple:
                        if item[0] == TokenType.FUNCTION:
                                window.add(
                                        item[1],
                                        color=Highlight.FUNCTION,
                                        refresh=False
                                )
                        if item[0] == TokenType.STRING:
                                window.add(
                                        item[1],
                                        color=Highlight.STRING,
                                        refresh=False
                                )
                else:
                        window.add(item, refresh=False)

def init_stringio():
        '''
        重定向 stdio 到 StringIO
        '''
        import sys
        from io import StringIO
        stringio = StringIO()
        sys.stdout = stringio
        return stringio

def get_redirected_std_output(stringio) -> str:
        redirected_std_output = stringio.getvalue()
        if redirected_std_output != '':
                return redirected_std_output + '\n'
        else:
                return ''

def crop_precolored_content(content: PrecoloredContent, maxwidth: int, nlines: int):
        '''
        这个算法在应对 tuple 中 field 第一个字符是 '\n' 的场景时可能会出问题，我没测试过
        '''
        import collections
        cropped_content = collections.deque()
        now_width = 0
        for field in reversed(content):
                if type(field) == tuple:
                        temp = ''
                        for ch in reversed(field[1]):
                                if ch == '\n':
                                        nlines -= 1
                                        if nlines == 0:
                                                cropped_content.appendleft((field[0], temp))
                                                return list(cropped_content)
                                        now_width = 0
                                        temp += ch
                                else:
                                        now_width += 1
                                        if now_width == maxwidth:
                                                nlines -= 1
                                        if now_width > maxwidth:
                                                now_width = now_width - maxwidth
                                                nlines -= 1
                                        if nlines == 0:
                                                cropped_content.appendleft((field[0], temp))
                                                return list(cropped_content)
                                        temp += ch
                        cropped_content.appendleft((field[0], temp[::-1]))
                else:
                        for ch in reversed(field):
                                if ch == '\n':
                                        nlines -= 1
                                        if nlines == 0:
                                                return list(cropped_content)
                                        now_width = 0
                                        cropped_content.appendleft(ch)
                                else:
                                        now_width += 1
                                        if now_width == maxwidth:
                                                nlines -= 1
                                        if now_width > maxwidth:
                                                now_width = now_width - maxwidth
                                                nlines -= 1
                                        if nlines == 0:
                                                return list(cropped_content)
                                        cropped_content.appendleft(ch)
        return list(cropped_content)

def process_content(
                content: str,
                last_time_content: PrecoloredContent,
                multilines: bool) -> PrecoloredContent:
        redirected_stdio = init_stringio() # 把 eval 的 stdio 重定向到这里

        outputwindow.clear()

        precolored_output : PrecoloredContent = last_time_content
        try:
                preprocessed = preprocess(content, redirected_stdio)
                if preprocessed == None:
                        result = ''
                else:
                        if multilines == True:
                                exec(preprocessed, {}, LOCALS)
                                result = str(LOCALS.get('_', None))
                        else:
                                result = str(eval(preprocessed, {}, LOCALS))
        except Exception as e:
                precolored_output.append((TokenType.FUNCTION, '! ' + str(e) + '\n'))
        else:
                precolored_output.append(get_redirected_std_output(redirected_stdio))
                if result != '':
                        precolored_output.append((TokenType.FUNCTION, result + '\n'))

        cropped_precolored_output = crop_precolored_content(
                precolored_output, outputwindow.get_width(), outputwindow.get_height())
        highlight_from_precolored_content(outputwindow, cropped_precolored_output)

        outputwindow.refresh()

        return precolored_output

class VariableVisitor(ast.NodeVisitor):
        def __init__(self):
                self.tokens : PrecoloredContent = []

        def visit_Name(self, node):
                import builtins
                # 将变量名添加到集合中
                if hasattr(builtins, node.id):
                        self.tokens.append(
                                (node.lineno,
                                 node.col_offset,
                                 TokenType.FUNCTION,
                                 node.id))

        def visit_Constant(self, node):
                if type(node.value) == str:
                        self.tokens.append(
                                (node.lineno,
                                 node.col_offset + 1,
                                 TokenType.STRING,
                                 node.value))

def line_col_to_len(content, lctoken, offset):
        (line, col, tt, value) = lctoken
        nowlen = 0
        for i in range(0, line - 1):
                nowlen += len(content[i]) + 1 # 包含换行符
        return nowlen + col + offset

def parse_tokens(content: str) -> list[tuple[int, TokenType, str]]:
        splitted_content = content.splitlines()

        # 删除反斜杠
        if splitted_content[0][0] == '\\':
                splitted_content[0] = splitted_content[0][1:]
                offset = 1
        else:
                offset = 0

        if splitted_content[-1][-1] == '\\':
                splitted_content[-1] = splitted_content[-1][:-1]

        try:
                tree = ast.parse('\n'.join(splitted_content))
        except SyntaxError as e:
                raise e
        visitor = VariableVisitor()
        visitor.visit(tree)
        linear_tokens = []
        for e in visitor.tokens:
                linear_tokens.append(
                        (line_col_to_len(splitted_content, e, offset),
                         e[2],
                         e[3]))
        return linear_tokens

def precolor_input_content(
                content: str,
                tokens : list[tuple[int, TokenType, str]]
            ) -> PrecoloredContent:

        precolored_content = list(content)
        replacing_offset = 0
        for (col_offset, tokentype, nodeid) in tokens:
                col_offset -= replacing_offset
                precolored_content[
                                         col_offset:col_offset+len(nodeid)
                                  ] = [(tokentype, nodeid)]
                replacing_offset += len(nodeid) - 1
        return precolored_content

def highlight_from_content(content: str):

        try:
                tokens = parse_tokens(content)
        except:
                return

        precolored_content = precolor_input_content(content, tokens)

        inputwindow.clear()

        highlight_from_precolored_content(inputwindow, precolored_content)

        inputwindow.refresh()

def highlight():
        real_point = inputwindow.get_point()

        highlight_from_content(inputwindow.get_content())

        inputwindow.set_point(real_point)       # 清除输出字符时对鼠标指针的所有副作用

history_input: list[str] = []
last_contents_point = 0

current_outputwindow_content: PrecoloredContent = []
allow_multilines = False
# 光标指向下一个字符，而非当前字符
while ch := inputwindow.wait_char():
        if ch == SpecialKey.LEFT:
                inputwindow.add_point(-1)
        elif ch == SpecialKey.RIGHT:
                if inputwindow.get_point() < len(inputwindow.get_content()):
                        inputwindow.add_point(1)
                else:
                        pass    # 之后的代码都假定光标在已写入的范围内
        elif ch == SpecialKey.BACKSPACE:
                content_list = inputwindow.get_content_chlist()
                if content_list != [] and inputwindow.get_point() != 0:
                        real_point = inputwindow.get_point()
                        inputwindow.add_point(-1)
                        content_list.pop(real_point - 1)
                        inputwindow.put(
                                content_list,
                                move_cur=False,
                                move_real_point=False)
                        highlight()
                else:
                        pass
        elif ch == SpecialKey.UP and history_input != [] and last_contents_point > 0:
                last_contents_point -= 1
                len_ = len(history_input[last_contents_point].splitlines())
                if len_ > 1:
                        allow_multilines = True
                else:
                        allow_multilines = False
                inputwindow.add_begin_y(len_ - inputwindow.get_height())
                inputwindow.put(history_input[last_contents_point])
                highlight()
        elif (ch == SpecialKey.DOWN
              and history_input != []):
                if last_contents_point < len(history_input) - 1:
                        last_contents_point += 1
                        len_ = len(history_input[last_contents_point].splitlines())
                        if len_ > 1:
                                allow_multilines = True
                        else:
                                allow_multilines = False
                        inputwindow.add_begin_y(len_ - inputwindow.get_height())
                        inputwindow.put(history_input[last_contents_point])
                        highlight()
                elif last_contents_point == len(history_input) - 1:
                        allow_multilines = False
                        last_contents_point += 1
                        inputwindow.clear()
                        highlight()
                else:
                        pass
        elif ch == SpecialKey.ENTER or ch == '\n':
                content = inputwindow.get_content()
                if len(content) != 0:
                        if content == 'exit':
                                break
                        elif content == 'cls':
                                outputwindow.clear()
                                history_input.append('cls')
                                last_contents_point += 1
                                inputwindow.clear()
                        elif content[-1] == '\\' and allow_multilines:
                                multilines_cache = content[1:-1]
                                current_outputwindow_content = (
                                        process_content(
                                                multilines_cache,
                                                current_outputwindow_content,
                                                True))
                                inputwindow.clear()
                                history_input.append(content)
                                last_contents_point += 1
                                inputwindow.set_begin_y(0)
                                allow_multilines = False
                        elif content[0] == '\\':
                                allow_multilines = True
                                inputwindow.add_begin_y(1)
                                inputwindow.add('\n')
                        elif allow_multilines:
                                inputwindow.insert(inputwindow.get_point(), '\n')
                        else:
                                current_outputwindow_content = (
                                        process_content(content,
                                                        current_outputwindow_content,
                                                        False))
                                inputwindow.clear()
                                history_input.append(content)
                                last_contents_point += 1
        elif ch == '(':
                inputwindow.insert(inputwindow.get_point(), "()")
                inputwindow.add_point(1)
                highlight()
        elif ch == '"':
                inputwindow.insert(inputwindow.get_point(), "\"\"")
                inputwindow.add_point(1)
                highlight()
        elif ch == '\'':
                inputwindow.insert(inputwindow.get_point(), "''")
                inputwindow.add_point(1)
                highlight()
        elif ch == '\t':
                inputwindow.insert(inputwindow.get_point(), "    ")
                inputwindow.add_point(4)
                highlight()
        elif ch == '\033':      # ESC 键
                break
        elif ch == SpecialKey.RESIZE:

                inputwindow.set_begin_xy(0, 0)
                inputwindow.set_end_xy(-1, 0)

                outputwindow.set_begin_xy(0, -1)
                outputwindow.set_end_xy(-1, 1)
        elif type(ch) == SpecialKey:    # 暂且忽略其它控制字符
                pass
        else:
                inputwindow.insert(inputwindow.get_point(), ch)
                inputwindow.add_point(1)
                highlight()

Screen.exit()

print("Pyeony safe-exited")