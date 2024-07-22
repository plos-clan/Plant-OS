qemu-system-i386 -d in_asm -net nic,model=pcnet -net user -serial stdio -device sb16 -device floppy -fda disk.img -boot a -m 256
