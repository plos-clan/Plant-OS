qemu-system-i386 \
  -cpu qemu64 \
  -smp sockets=1,cores=2,threads=2,maxcpus=4 \
  -bios seabios.bin \
  -m 512 \
  -device virtio-mouse-pci \
  -vga std -global VGA.vgamem_mb=32 \
  -net nic,model=pcnet -net user \
  -serial stdio \
  -device sb16 -device intel-hda -device hda-micro \
  -device floppy,drive=fda -drive id=fda,if=none,format=raw,file=disk.img \
  -device ide-hd,drive=hda,bus=ide.0 -drive id=hda,if=none,format=raw,file=hd.img \
  -boot a \
  $*

echo -n -e "\033[T\033[2K\033[999D"
