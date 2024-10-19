qemu-system-x86_64 \
  -bios OVMF.fd \
  -m 1024 \
  -net none \
  -drive file=fat:rw:plos-efi,index=0,format=vvfat \
  $*
