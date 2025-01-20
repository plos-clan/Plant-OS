#!/usr/bin/sh

objcopy --only-keep-debug loader.bin loader.sym
objcopy --strip-debug loader.bin
objcopy --only-keep-debug kernel.bin kernel.sym
objcopy --strip-debug kernel.bin
# 上面几步把调试信息分离，防止炸软盘

qemu-img create disk.img 1474560
mformat -f 1440 -B boot.bin -i disk.img
mcopy -i disk.img ./loader.bin ::
mcopy -i disk.img ./kernel.bin ::
mcopy -i disk.img ./v86_service.bin ::
mcopy -i disk.img ./ld-plos.bin ::

qemu-img create hd.img 64M
mformat -t 64 -h 64 -i hd.img
mcopy -i hd.img ./disk.img ::
mcopy -i hd.img ./resource/audio.plac ::
mcopy -i hd.img ./resource/world_execute_me.qoa ::
mcopy -i hd.img ./resource/do_you_hear_the_people_sing.qoa ::
mcopy -i hd.img ./resource/test.qoi ::
mcopy -i hd.img ./resource/魔女之旅_片头_リテラチュア.mp3 ::/Wandering_Witch.mp3
mcopy -i hd.img ./resource/plos_start.mp3 ::

for file in ./app/*; do
  mcopy -i hd.img "$file" ::
done

qemu-img create hd1.img 128M
mformat -t 128 -h 64 -i hd1.img
mcopy -i hd1.img ./resource/font1.plff ::
mcopy -i hd1.img ./resource/font2.plff ::

# 用于在 plos 开机时自动运行程序
echo -n '
echo Hello, world!
/fatfs1/testapp.bin
/fatfs1/testapp-cpp.bin
/fatfs1/pf.bin
/fatfs1/plos_start.mp3
' >./autorun.txt
mcopy -i disk.img ./autorun.txt ::
rm ./autorun.txt

# 移动文件
mkdir -p ./img
rm ./img/*.img
mv disk.img ./img
mv hd.img ./img
mv hd1.img ./img
