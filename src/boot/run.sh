qemu-system-i386 -net nic,model=pcnet -net user -serial stdio -device sb16 -device floppy -fda disk.img -boot a -m 256
