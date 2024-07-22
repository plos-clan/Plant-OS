import os
import subprocess
def run(command, debug = False):
        if debug:
                os.system(command)
        else:
                subprocess.Popen(command.split(" "))
def build(cc, nasm_path = "nasm", debug = False):
        cc += ' -Iinclude/loader'
        for root, dirs, files in os.walk('src/loader', topdown=False):
                for file in files:
                        filebasename, fileext = os.path.splitext(file)
                        if fileext == '.c':
                                if not os.path.exists('build/' + root):
                                        os.makedirs('build/' + root)
                                os.system(
                                        f"{cc} -c {root + '/' + file} -o build/{root + '/' + filebasename}.o")
                        elif fileext == '.asm':
                                if not os.path.exists('build/' + root):
                                        os.makedirs('build/' + root)
                                os.system(
                                        f"{nasm_path} -f elf {root + '/' +  file} -o build/{root + '/' +  filebasename}.o"
                                )