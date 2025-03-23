Here is a qemu disk image. Just unzip and:

qemu -L pc-bios -hda xlinux-clean.img -no-reboot -m 128 -k en-us
Newer: qemu-system-i386 -boot c -hda xlinux-clean.img -m 128

Ist also has already c245.tgz = C compiler installed with some source file in /usr/src/c and /root/c.

---

For virtualbox:

1. Extract 7z parts to get xlinux.img
2. Right click on xlinux.img -> einhängen (mount).
3. mount -> show where mountet (ex: /dev/loop14p1)
4. Create vmdk file: VBoxManage internalcommands createrawvmdk -filename ./xlinux.vmdk -rawdisk /dev/loop14p1
5. Edit xlinux.vmdk -> replace "/dev/loop14p1" with "./xlinux.img"

Just in case you need to remove hdd (moved to other place, etc.):
1. remove hdd from virtual machine
2. vboxmanage list hdds -> see hdds and uuids
3. vboxmanage closemedium disk <uuid> --delete
4. Move vmdk and img; attach to virtual machine.
  
  
