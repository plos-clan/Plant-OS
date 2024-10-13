qemu-system-i386 \
  -smp sockets=1,cores=4,threads=2,maxcpus=8 \
  -bios seabios.bin \
  -net nic,model=pcnet -net user \
  -serial stdio \
  -device sb16 -device intel-hda -device hda-micro \
  -device floppy,drive=fda -drive id=fda,if=none,format=raw,file=disk.img \
  -device ide-hd,drive=hda,bus=ide.0 -drive id=hda,if=none,format=raw,file=hd.img \
  -boot a \
  -m 256 \
  $*
