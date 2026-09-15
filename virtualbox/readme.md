Hope that I can upload 29MB ova file here.

Nope: Yowza, that’s a big file. Try again with a file smaller than 25MB.

Splitted with: ./7zz a -tzip -v20m xlinux_virtualbox.zip xlinux.ova

Unzip into ova file with: ./7zz x xlinux_virtualbox.zip.001

---

Convert from raw/img to vdi: VBoxManage convertfromraw inputfile.img outputfile.vdi --format VDI

Convert from vdi to raw/img: VBoxManage clonemedium --format RAW inputfile.vdi outputfile.img

I could write such img file with dd to a real hdd (CF card with adapter) and boot from it.

---

This is already done and included in ova:

```
start xlinux. login root.
cd /
mount -t msdos /dev/fd0 /mnt
tar zxvf /mnt/xdev20.tgz
cd /usr/src/c
tar zxvf /mnt/x11test.tgz
tar zxvf /mnt/c_banks.tgz - ioccc 1998 deobfuscated flight simulator. start with ./run2.sh pittsbur
```

