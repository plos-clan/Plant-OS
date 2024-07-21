r'''
Non-self-compiling auto build RePlantOS

  build                 build RePlantOS minimally
  build-doc             build RePlantOS document
  clear                 clear `build` directory
  init                  initation RePlantOS workspace

options:
  -h, --help            show this help message and exit
  --debug               show debug informations
  --nasm-path NASM_PATH
                        set NASM compiler path
'''
import argparse
import sys
import os
import shutil

sys.dont_write_bytecode = True # 禁止其生成 __pycache__ 文件夹

parser = argparse.ArgumentParser(
        prog="RePlantOS Make", description="Non-self-compiling auto build RePlantOS"
)

parser.add_argument("branch_arg", nargs="?")

parser.add_argument("--debug", action="store_true", help="show debug informations")
parser.add_argument("--nasm-path", type=str, help="set NASM compiler path", default='nasm')
args = parser.parse_args()

def build_boot():
        import src.boot.make
        print("Building bootstrap...")
        try:
                src.boot.make.build(args.nasm_path, args.debug)
        except Exception as e:
                print("Error: ", end="")
                print(e)
                exit()
        else:
                print("...Ok")

def build_loader():
        pass

def build_kernel():
        pass

def build():
        if not os.path.exists("build"):
                os.mkdir("build")
        build_boot()
        build_loader()
        build_kernel()

        with open('build/PlantOS.img.tmp', 'wb+') as f:
                f.write(1024 * 1440 * b'\0')
        os.system("mformat -f 1440 -B build/boot.bin -i build/PlantOS.img.tmp")
        os.system("mcopy -i build/PlantOS.img.tmp src/loader/out/dosldr.bin ::")
        os.rename("build/PlantOS.img.tmp", "build/PlantOS.img")

def clear():
        shutil.rmtree("build")

def init():
        os.mkdir("build")

def build_doc():
        import tools.intromark
        tools.intromark.compile_all_subprojects_intromark()
        tools.intromark.compile('README.md')

if args.branch_arg == 'build':
        build()
elif args.branch_arg == 'build-doc':
        build_doc()
elif args.branch_arg == 'clear':
        clear()
elif args.branch_arg == 'init':
        init()
elif args.branch_arg == 'repl':
        os.system('python tools/repl.py')
elif args.branch_arg == 'call':
        os.system('python tools/call.py')
else:
        print('Wrong parameter!\n')
        parser.print_help()