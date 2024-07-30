qemu-img create disk.img 1474560
mformat -f 1440 -B boot.bin -i disk.img
mcopy -i disk.img loader.bin ::
mcopy -i disk.img kernel.bin ::
mcopy -i disk.img testapp.bin ::
