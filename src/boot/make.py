import os
import subprocess
def run(command, debug = False):
        if debug:
                os.system(command)
        else:
                subprocess.Popen(command.split(" "))
def build(nasm = "nasm", debug = False):
        run(f"{nasm} {os.path.dirname(__file__)}/mbrboot.asm -o build/boot.bin", debug)