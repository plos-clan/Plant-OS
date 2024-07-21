'''
You should call this program in root directory of RePlantOS,
if not, some directory (like "build") will be generated in non-root directory.
Usage:
        python tools/call.py boot build
                        --- Call a function from boot/make.py
        python tools/call.py boot build -h
                        --- Get the document string about build()
        python tools/call.py boot build --nasm-path="nasm"
                        --- Let nasm_path of "def build(nasm_path)" is "nasm"
        python tools/call.py xxx a-b
                        --- Call a_b()
        python tools/call.py boot build @debug
                        --- Call a function with a boolean flag
                                (for this example. is "debug = True")
        python tools/call.py boot/make.py build
                        --- Just call a file
'''
import sys
import importlib
import os

sys.dont_write_bytecode = True

dir_path = os.path.dirname(os.path.realpath(__file__))
parent_dir_path = os.path.abspath(os.path.join(dir_path, os.pardir))
sys.path.insert(0, parent_dir_path)

def process_arg_name(fn, no_prefix = False):
        """'--nasm-path' -> 'nasm_path'"""
        new_fn = ""
        for i, c in enumerate(fn):
                if no_prefix:
                        pass
                elif i < 2:
                        continue

                if c == '-':
                        new_fn += '_'
                else:
                        new_fn += c
        return new_fn

if len(sys.argv) < 3:
        print("Too few parameters")
        exit()

modelname = ''
for ch in sys.argv[1]:
        if ch == '/':
                modelname += '.'
        else:
                modelname += ch

if modelname[-3:] == '.py':
        model = importlib.import_module(modelname[:-3])
else:
        model = importlib.import_module(modelname + ".make")

funcname = ""
for e in sys.argv[2]:
        if e == '-':
                funcname += '_'
        else:
                funcname += e

if len(sys.argv) == 4 and (sys.argv[3] == "-h" or sys.argv[3] == "--help"):
        print(eval("model." + funcname).__doc__)
        exit()

args = ""
cache = None
for e in sys.argv[3::]:
        if args != "":
                args += ','

        try:
                (a, b) = e.split('=', 1)
        except ValueError:
                if e[0] == "@":
                        args += process_arg_name(e[1::], no_prefix=True) + "=True"
                if cache == None:
                        cache = e
                else:
                        args += process_arg_name(cache) + "=\"" + e + "\""
                        cache = None
        else:
                args += process_arg_name(a) + "=\"" + b + "\""
eval("model." + funcname + '(' + args + ')')