import os
import subprocess
def run(command, debug = False):
        if debug:
                os.system(command)
        else:
                subprocess.Popen(command.split(" "))
def build(cc, nasm = "nasm", ar = "ar", debug = False):
        cc += ' -Iinclude/libc'
        for root, dirs, files in os.walk('src/libc', topdown=False):
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
        for root, dirs, files in os.walk('build/src/libc'):
                for filename in files:
                        filebasename, fileext = os.path.splitext(filename)
                        if fileext == '.o':
                                objfile_list.append(root + '/' + filename)
        retcode = os.system(
                f"{ar} rv build/src/libc/libc.a {' '.join(objfile_list)}"
        )
        if retcode > 0:
                raise "Linker Error"