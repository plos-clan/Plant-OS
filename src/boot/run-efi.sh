qemu-system-x86_64 \
  -smp sockets=1,cores=2,threads=2,maxcpus=4 \
  -bios OVMF.fd \
  -m 1024 \
  -vga std -global VGA.vgamem_mb=32 \
  -net none \
  -serial stdio \
  -device sb16 -device intel-hda -device hda-micro \
  -drive file=fat:rw:plos-efi,index=0,format=vvfat \
  $*
