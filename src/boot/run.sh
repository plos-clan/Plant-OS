#!/usr/bin/sh

qemu-system-i386 \
  -cpu qemu64 \
  -smp sockets=1,cores=2,threads=2,maxcpus=4 \
  -m 512 \
  -device virtio-mouse-pci \
  -device virtio-keyboard-pci \
  -device virtio-gpu-pci,hostmem=128M \
  -vga virtio -global VGA.vgamem_mb=32 \
  -net nic,model=pcnet -net user \
  -serial stdio \
  -device sb16 -device intel-hda -device hda-micro \
  -device floppy,drive=fda -drive id=fda,if=none,format=raw,file=img/disk.img \
  -device ide-hd,drive=hda,bus=ide.0 -drive id=hda,if=none,format=raw,file=img/hd.img \
  -drive id=disk,if=none,format=raw,file=img/hd1.img \
  -device ahci,id=ahci \
  -device ide-hd,drive=disk,bus=ahci.0 \
  -boot a \
  $*

echo -n -e "\033[m\033[2K\033[A\033[2K\033[999D"
