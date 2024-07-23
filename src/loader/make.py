import os
import subprocess
def run(command, debug = False):
        if debug:
                os.system(command)
        else:
                subprocess.Popen(command.split(" "))
def build(cc, nasm = "nasm", ld = "ld", debug = False):
        cc += ' -Iinclude/loader'
        for root, dirs, files in os.walk('src/loader', topdown=False):
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
                                with open('build/' + root + '/' + filename, 'r') as bakfile:
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
        for root, dirs, files in os.walk('build/src/loader'):
                for filename in files:
                        filebasename, fileext = os.path.splitext(filename)
                        if fileext == '.o':
                                objfile_list.append(root + '/' + filename)

        retcode = os.system(
                f"{ld} -Ttext 0x100000 -m elf_i386 -e loader_main {' '.join(objfile_list)} build/src/libc/libc.a -o build/src/loader/dosldr.bin"
        )
        if retcode > 0:
                raise "Linker Error"