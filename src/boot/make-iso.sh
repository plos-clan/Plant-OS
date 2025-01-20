#!/usr/bin/sh

mkdir -p plos-iso/boot/grub
cp kernel.bin plos-iso/boot/kernel.bin
echo 'set timeout=3
menuentry "Plant-OS" {
    multiboot2 /boot/kernel.bin
}
' >plos-iso/boot/grub/grub.cfg

grub-mkrescue -o img/plos.iso plos-iso
