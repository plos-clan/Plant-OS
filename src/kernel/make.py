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

                                # 增量编译，如果没变化就不编译
                                with open('build/' + root + '/' + filename, 'r') as bakfile:
                                        with open(root + '/' + filename,'r') as srcfile:
                                                if bakfile.read() == srcfile.read():
                                                        continue

                                retcode = os.system(
                                        f"{cc} -c {root + '/' + filename} -o build/{root + '/' + filebasename}.o")
                                if retcode > 0:
                                        raise "C Compiler Error"

                                # 如果编译没问题就缓存当前文件
                                with open('build/' + root + '/' + filename,'w') as bakfile:
                                        with open(root + '/' + filename,'r') as srcfile:
                                                bakfile.write(srcfile.read())
                        elif fileext == '.asm':
                                if not os.path.exists('build/' + root):
                                        os.makedirs('build/' + root)

                                # 增量编译，如果没变化就不编译
                                with open('build/' + root + '/' + filename,'r') as bakfile:
                                        with open(root + '/' + filename,'r') as srcfile:
                                                if bakfile.read() == srcfile.read():
                                                        continue
                                retcode = os.system(
                                        f"{nasm} -f elf {root + '/' +  filename} -o build/{root + '/' +  filebasename}.o"
                                )
                                if retcode > 0:
                                        raise "NASM Error"

                                # 如果编译没问题就缓存当前文件
                                with open('build/' + root + '/' + filename,'w') as bakfile:
                                        with open(root + '/' + filename,'r') as srcfile:
                                                bakfile.write(srcfile.read())

        objfile_list = []
        for root, dirs, files in os.walk('build/src/kernel'):
                for filename in files:
                        filebasename, fileext = os.path.splitext(filename)
                        if fileext == '.o':
                                objfile_list.append(root + '/' + filename)
        retcode = os.system(
                f"{ld} -m elf_i386 -Ttext 0x280000 -e kernel_main {' '.join(objfile_list)} build/src/base/base.a -o build/src/kernel/kernel.bin"
        )
        if retcode > 0:
                raise "Linker Error"