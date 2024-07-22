import os
import subprocess
def run(command, debug = False):
        if debug:
                os.system(command)
        else:
                subprocess.Popen(command.split(" "))
def build(cc, nasm = "nasm", ld = "ld", debug = False):
        cc += ' -Iinclude/kernel'
        for root, dirs, files in os.walk('src/kernel', topdown=False):
                for filename in files:
                        filebasename, fileext = os.path.splitext(filename)
                        if fileext == '.c':
                                if not os.path.exists('build/' + root):
                                        os.makedirs('build/' + root)
                                retcode = os.system(
                                        f"{cc} -c {root + '/' + filename} -o build/{root + '/' + filebasename}.o")
                                if retcode > 0:
                                        raise "C Compiler Error"
                        elif fileext == '.asm':
                                if not os.path.exists('build/' + root):
                                        os.makedirs('build/' + root)
                                retcode = os.system(
                                        f"{nasm} -f elf {root + '/' +  filename} -o build/{root + '/' +  filebasename}.o"
                                )
                                if retcode > 0:
                                        raise "NASM Error"

        objfile_list = []
        for root, dirs, files in os.walk('build/src/kernel'):
                for filename in files:
                        filebasename, fileext = os.path.splitext(filename)
                        if fileext == '.o':
                                objfile_list.append(root + '/' + filename)
        retcode = os.system(
                f"{ld} -m elf_i386 -Ttext 0x280000 -e kernel_main {' '.join(objfile_list)} build/src/libc/libc.a -o build/src/kernel/kernel.bin"
        )
        if retcode > 0:
                raise "Linker Error"