objcopy --only-keep-debug kernel.bin kernel.sym
objcopy --strip-debug kernel.bin

qemu-img create disk.img 1474560
mformat -f 1440 -B boot.bin -i disk.img
mcopy -i disk.img loader.bin ::
mcopy -i disk.img kernel.bin ::

qemu-img create hd.img 128M
mformat -t 64 -h 64 -i hd.img
mcopy -i hd.img testapp.bin ::
mcopy -i hd.img testapp-cpp.bin ::
mcopy -i hd.img font1.plff ::
mcopy -i hd.img font2.plff ::
mcopy -i hd.img audio.plac ::
mcopy -i hd.img world_execute_me.qoa ::
mcopy -i hd.img do_you_hear_the_people_sing.qoa ::

