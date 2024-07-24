#!/bin/env python
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
        cc = f"clang -m32 -I{os.path.realpath('include')} -c \
-nostdinc -nostdlib \
-ffreestanding -fno-stack-protector -Qn \
-fno-pic -fno-pie -fno-asynchronous-unwind-tables -fomit-frame-pointer \
-O1 \
-finput-charset=UTF-8 -fexec-charset=UTF-8 -mno-mmx -mno-sse"
elif args.cc == 'gcc':
        cc = f"gcc -m32 -I{os.path.realpath('include')} -c \
-nostdinc -nolibc -nostdlib \
-ffreestanding -fno-stack-protector -Qn \
-fno-pic -fno-pie -fno-asynchronous-unwind-tables -fomit-frame-pointer \
-O1 \
-finput-charset=UTF-8 -fexec-charset=UTF-8 -mno-mmx -mno-sse"
else:
        cc = args.cc

def print_build_success():
        print("\033[32;1mBUILD SUCCESS\033[0m")

def print_build_failed():
        print("\033[31;1mBUILD FAILED\033[0m")

def print_success():
        print("\033[32;1mSUCCESS\033[0m")

def print_failed():
        print("\033[31;1mFAILED\033[0m")

def print_little_success():
        print("\033[32;1msuccess\033[0m")

def print_little_failed():
        print("\033[31;1mfailed\033[0m")

def build_boot():
        import src.boot.make
        print("Building bootstrap...")
        try:
                src.boot.make.build(
                        nasm=args.nasm_path,
                        debug=args.debug)
        except Exception as e:
                print_build_failed()
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
                print_build_failed()
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
                print_build_failed()
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
                print_build_failed()
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

        print_build_success()

def clean():
        shutil.rmtree("build")

def init():
        os.mkdir("build")
        for root, dirs, files in os.walk('src'):
                for file in files:
                        os.makedirs('build/' + root, exist_ok=True)
                        with open('build/' + root + '/' + file,'w') as bakfile:
                                bakfile.write('')

def build_doc():
        import tools.intromark
        tools.intromark.compile_all_subprojects_intromark()
        tools.intromark.compile('README.md')

def scan_files(dirname) -> str:
        ret = []
        for root, dirs, files in os.walk(dirname):
                for file in files:
                        ret.append(root + '/' + file)
        return ret

def build_lsp_hints():
        import json
        content = []
        for filename in scan_files('src'):
                filebasename, fileext = os.path.splitext(filename)
                if fileext != '.c' and fileext != '.cpp':
                        continue
                realpathfilename = os.path.realpath(filename)

                subprojectname = filename.split('/')[1]

                if os.path.exists('include/' + subprojectname):
                        content.append({
                                "directory": os.path.realpath('build'),
                                "command": f"\
{cc} -o {filebasename}.o {realpathfilename} -I{os.path.realpath('include/' + subprojectname)}",
                                "file": realpathfilename,
                                "output": filebasename + ".o"
                        })
                else:
                        content.append({
                                "directory": os.path.realpath('build'),
                                "command": f"{cc} -o {filebasename}.o {realpathfilename}",
                                "file": realpathfilename,
                                "output": filebasename + ".o"
                        })
        with open('build/compile_commands.json','w') as file:
                file.write(json.dumps(content, indent=8))

def run():
        os.system(f"\
{args.qemu} \
-net nic,model=pcnet -net user \
-serial stdio -device sb16 -device floppy -fda build/PlantOS.img -boot a -m 256")

def test():
        import tools.unitest
        files = tools.unitest.build_all_files()
        fail_list = []
        for file in files:
                splitted_path = file.split('/')
                origin_filename = ('/'.join(splitted_path[2:-1])
                      + '/'
                      + '-'.join(splitted_path[-1].split('-')[:-1])
                      + '.c')
                nth = str(int(splitted_path[-1].split('-')[-1].split('.')[0]) + 1) + 'st'
                print('Testing '
                      + origin_filename
                      + ' '
                      + nth
                      + ' ... ',
                      end="")
                os.system(f"{cc.split()[0]} -I{os.path.realpath('.')} {file} -o {os.path.splitext(file)[0]}")
                originretcode = os.system(f'./{os.path.splitext(file)[0]}')      # 高8位是返回值 低8位是非正常退出时的状态码
                retcode = originretcode >> 8
                if retcode == 0 and originretcode == 0:
                        print_little_success()
                elif retcode == 0 and originretcode != 0:
                        # 非正常退出
                        print_little_failed()
                        fail_list.append((1, 0, origin_filename, nth))
                else:
                        # 正常退出但返回值不正常
                        print_little_failed()
                        fail_list.append((0, retcode, origin_filename, nth))
        if fail_list == []:
                print_success()
        else:
                print_failed()
                for type_, retcode, origin_filename, nth in fail_list:
                        if type_ == 0:
                                print(f"{origin_filename} {nth} returned code: {str(retcode)}")
                        else:
                                print(f"{origin_filename} {nth} abnormal exit")


if args.branch_arg == 'build':
        build()
elif args.branch_arg == 'build-doc':
        build_doc()
elif args.branch_arg == 'clean':
        clean()
elif args.branch_arg == 'init':
        init()
elif args.branch_arg == 'repl':
        os.system('python tools/repl.py')
elif args.branch_arg == 'call':
        os.system('python tools/call.py')
elif args.branch_arg == 'build-lsp-hints':
        build_lsp_hints()
elif args.branch_arg == 'run':
        build()
        run()
elif args.branch_arg == 'test':
        test()
else:
        print('Wrong parameter!\n')
        parser.print_help()