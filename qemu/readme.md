Here is a qemu disk image. Just unzip and:

qemu -L pc-bios -hda xlinux-clean.img -no-reboot -m 128 -k en-us

Newer: qemu-system-i386 -boot c -hda xlinux-clean.img -m 128

Ist also has already c245.tgz = C compiler installed with some source file in /usr/src/c and /root/c.

NEW 2025:
After app. 30 years, we now can have xlib developments.
```
In host linux box with qemu installed:
Create floppy disk image: mkfs.msdos -C floppy.img 1440
Mount floppy image and copy all 3 tgz files into it: mkdir mnt. sudo mount -t msdos -o loop floppy.img mnt. cp *.tgz mnt
umount mnt
qemu-system-i386 -boot c -hda xlinux-clean.img -m 128 -fda floppy.img -vga cirrus


start xlinux. login root. 
mount -t msdos /dev/fd0 /mnt
cd /
tar zxvf xdev20.tgz
cd /usr/src/c
tar zxvf /mnt/x11test.tgz
tar zxvf /mnt/c_banks.tgz - ioccc 1998 deobfuscated flight simulator. start with ./run.sh
```

---

For virtualbox:

1. Extract archive parts to get xlinux-clean.img
2. Right click on xlinux-clean.img -> einhängen (mount).
3. mount -> show where mountet (ex: /dev/loop14p1)
4. Create vmdk file: VBoxManage internalcommands createrawvmdk -filename ./xlinux.vmdk -rawdisk /dev/loop14p1
5. Edit xlinux.vmdk -> replace "/dev/loop14p1" with "./xlinux.img"

Just in case you need to remove hdd (moved to other place, etc.):
1. remove hdd from virtual machine
2. vboxmanage list hdds -> see hdds and uuids
3. vboxmanage closemedium disk <uuid> --delete
4. Move vmdk and img; attach to virtual machine.
  
  
