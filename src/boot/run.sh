qemu-system-i386 -smp sockets=1,cores=4,threads=2,maxcpus=8 -net nic,model=pcnet -net user -serial stdio -device sb16 -device floppy -fda disk.img -hda hd.img -boot a -m 256
