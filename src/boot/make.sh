#!/usr/bin/sh

objcopy --only-keep-debug loader.bin loader.sym
objcopy --strip-debug loader.bin
objcopy --only-keep-debug kernel.bin kernel.sym

rm ./kernel.zst
zstd -o kernel.zst kernel.bin

qemu-img create disk.img 64M
mformat -t 64 -h 64 -B boot.bin -i disk.img
mcopy -i disk.img ./loader.bin ::
mcopy -i disk.img ./kernel.zst ::
mcopy -i disk.img ./v86_service.bin ::
mcopy -i disk.img ./ld-plos.bin ::
mcopy -i disk.img ./kernel.bin ::
mcopy -i disk.img ./resource/audio.plac ::
mcopy -i disk.img ./resource/world_execute_me.qoa ::
mcopy -i disk.img ./resource/do_you_hear_the_people_sing.qoa ::
mcopy -i disk.img ./resource/test.qoi ::
mcopy -i disk.img ./resource/魔女之旅_片头_リテラチュア.mp3 ::/Wandering_Witch.mp3
mcopy -i disk.img ./resource/plos_start.mp3 ::

for file in ./app/*; do
  mcopy -i disk.img "$file" ::
done

qemu-img create disk2.img 128M
mformat -t 128 -h 64 -i disk2.img
mcopy -i disk2.img ./resource/font1.plff ::
mcopy -i disk2.img ./resource/font2.plff ::

# 用于在 plos 开机时自动运行程序
echo -n '
echo Hello, world!
/fatfs0/testapp.bin
/fatfs0/testapp-cpp.bin
/fatfs0/pf.bin
/fatfs0/plos_start.mp3
' >./autorun.txt
mcopy -i disk.img ./autorun.txt ::
rm ./autorun.txt

# 移动文件
mkdir -p ./img
rm ./img/*.img
mv disk.img ./img
mv disk2.img ./img
