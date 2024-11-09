add_rules("mode.debug", "mode.release")

local ovmf_path = "/usr/share/OVMF/OVMF_CODE_4M.fd"
local subsystem = "efi-app"

local ldflags = ""

target("plos")
    set_kind("binary")

    add_cxflags("-c -g -ffreestanding -nostdlib -mcmodel=large -m64 -fdata-sections -ffunction-sections -Wall -Wextra -Werror")
    add_cflags("-std=c2x")
    set_optimize("fastest")

    add_includedirs("amd64/include")

    add_files("amd64/src/**.c")

    before_build("!linux|x86_64", function () 
        raise("invaild platform, please use linux or cross compiler instead")
    end)

    -- before_build(function () 
    --     if(subsystem == "") then
    --         raise("please replace \"subsystem\" with the target subsystem in xmake.lua")
    --     end

    --     os.tryrm("build/.objs/plos/linux/x86_64/debug/src/bin/container.S.o")

    --     os.tryrm("src/bin/ap_boot.bin")
    --     os.exec("nasm src/bin/ap_boot.asm -o src/bin/ap_boot.bin")
    -- end)

    on_link(function (target)
        objectdir = target:objectdir()
        rundir = target:rundir()
        name = target:name()
        compiler = target:tool("cxx")
        objcopy, _ = path.filename(compiler):gsub("gcc", "objcopy")
        objcopy = path.join(path.directory(compiler), objcopy)
    
        objlist = ""
        for key,val in pairs(target:objectfiles()) do 
            objlist = objlist..val.." "
        end
        
        -- link
        os.exec(compiler.." "..objlist.."-flto -nostdlib -nostartfiles -Tamd64/linker.ld -shared -Wl,-Bsymbolic,-Bstatic,-gc-sections "..ldflags.." -o "..objectdir.."/"..name..".so")
        
        -- generate efi image
        os.mkdir(rundir)
        
        os.exec(objcopy.." -j .text -j .data -j .rela.dyn -j .reloc --target efi-app-x86_64 --subsystem "..subsystem.." "..objectdir.."/"..name..".so "..rundir.."/"..name.."_unstrip.efi")
        os.exec(objcopy.." -R .reloc --strip-all "..rundir.."/"..name.."_unstrip.efi "..rundir.."/"..name.."_stripped.efi")
    end)

    on_run(function (target) 
        if(ovmf_path == "") then
            raise("please replace \"ovmf_path\" with your ovmf path in xmake.lua")
        end

        -- copy to build/esp
        rundir = target:rundir()
        name = target:name()
        espdir = rundir.."/../../../esp/"

        os.mkdir(espdir.."efi/boot/")
        os.trycp(rundir.."/"..name.."_stripped.efi", espdir.."efi/boot/bootx64.efi")

        os.exec("sudo qemu-system-x86_64 -enable-kvm -no-reboot -net none -m 6g -smp 4 -hda fat:rw:build/esp -serial stdio -pflash "..ovmf_path)
    end)