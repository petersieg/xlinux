Here is a qemu disk image. Just unzip and:

qemu -L pc-bios -hda xlinux-clean.img -no-reboot -m 128 -k en-us

Ist also has already c245.tgz = C compiler installed with some source file in /usr/src/c and /root/c.

---

For virtualbox:

1. Combine 7z parts: cat xlinux* > ./xlinux.img
2. Right click on xlinux.img -> einhängen (mount).
3. mount -> show where mountet (ex: /dev/loop14p1)
4. Create vmdk file: VBoxManage internalcommands createrawvmdk -filename ./xlinux.vmdk -rawdisk /dev/loop14p1
5. Edit xlinux.vmdk -> replace "/dev/loop14p1" with "./xlinux.img"

