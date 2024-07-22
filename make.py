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
parser.add_argument("--cc", type=str, help="set C compiler path", default='clang')
parser.add_argument("--ar", type=str, help="set AR like \"ar\" or \"llvm-ar\"", default='ar')
parser.add_argument("--ld", type=str, help="set Linker like \"ld\" or \"ld.lld\"", default='ld')
parser.add_argument("--qemu", type=str, help="set QEMU visual box like \"qemu-system-x86_64\"", default='qemu-system-x86_64')
parser.add_argument("--mformat-path", type=str, help="set mformat path", default='mformat')
parser.add_argument("--mcopy-path", type=str, help="set mcopy path", default='mcopy')

args = parser.parse_args()

if args.cc == 'clang':
        cc = "clang -m32 -Iinclude -c \
-nostdinc -nostdlib \
-ffreestanding -fno-stack-protector -Qn \
-fno-pic -fno-pie -fno-asynchronous-unwind-tables -fomit-frame-pointer \
-Oz \
-finput-charset=UTF-8 -fexec-charset=UTF-8 -mno-mmx -mno-sse"
elif args.cc == 'gcc':
        cc = "gcc -m32 -Iinclude -c \
-nostdinc -nolibc -nostdlib \
-ffreestanding -fno-stack-protector -Qn \
-fno-pic -fno-pie -fno-asynchronous-unwind-tables -fomit-frame-pointer \
-O0 \
-finput-charset=UTF-8 -fexec-charset=UTF-8 -mno-mmx -mno-sse"
else:
        cc = args.cc

def print_success():
    print("\033[32;1mBUILD SUCCESS\033[0m")

def print_error():
    print("\033[31;1mBUILD FAILED\033[0m")

def build_boot():
        import src.boot.make
        print("Building bootstrap...")
        try:
                src.boot.make.build(
                        nasm=args.nasm_path,
                        debug=args.debug)
        except Exception as e:
                print_error()
                print(e)
                exit()

def build_libc():
        import src.libc.make
        print("Building libc...")
        try:
                src.libc.make.build(cc,
                        nasm=args.nasm_path,
                        ar=args.ar,
                        debug=args.debug)
        except Exception as e:
                print_error()
                print(e)
                exit()

def build_loader():
        import src.loader.make
        print("Building loader...")
        try:
                src.loader.make.build(cc,
                                      nasm=args.nasm_path,
                                      ld=args.ld,
                                      debug=args.debug)
        except Exception as e:
                print_error()
                print(e)
                exit()

def build_kernel():
        import src.kernel.make
        print("Building kernel...")
        try:
                src.kernel.make.build(cc,
                                      nasm=args.nasm_path,
                                      ld=args.ld,
                                      debug=args.debug)
        except Exception as e:
                print_error()
                print(e)
                exit()

def build():

        if not os.path.exists("build"):
                os.mkdir("build")
        build_boot()
        build_libc()
        build_loader()
        build_kernel()

        with open('build/PlantOS.img.tmp', 'wb+') as f:
                f.write(1024 * 1440 * b'\0')
        os.system(f"{args.mformat_path} -f 1440 -B build/boot.bin -i build/PlantOS.img.tmp")
        os.system(f"{args.mcopy_path} -i build/PlantOS.img.tmp build/src/loader/dosldr.bin ::")
        os.system(f"{args.mcopy_path} -i build/PlantOS.img.tmp build/src/kernel/kernel.bin ::")
        os.rename("build/PlantOS.img.tmp", "build/PlantOS.img")

        print_success()

def clear():
        shutil.rmtree("build")

def init():
        os.mkdir("build")

def build_doc():
        import tools.intromark
        tools.intromark.compile_all_subprojects_intromark()
        tools.intromark.compile('README.md')

def run():
        os.system(f"{args.qemu} -net nic,model=pcnet -net user -serial stdio -device sb16 -device floppy -s -S -fda build/PlantOS.img -boot a -m 256")

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
elif args.branch_arg == 'run':
        build()
        run()
else:
        print('Wrong parameter!\n')
        parser.print_help()