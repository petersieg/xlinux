##!/bin/sh

echo "root::14729:0:99999:7:::" > /etc/shadow
echo "root:x:0:0:root:/root:/bin/sh" > /etc/passwd
echo "wwwrun:x:1:1:wwwrun:/root:/bin/sh" >> /etc/passwd
echo "wwwadmin:x:1:root,wwwrun" > /etc/group

cd /tmp
cp /flash2/vip1710.tar.bz2 /tmp
bunzip2 vip1710.tar.bz2
tar -x -f vip1710.tar
rm -f vip1710.tar

## usbstorage
mkdir /root
mkdir /lib/modules
mv scsi_mod.o /lib/modules/
mv sd_mod.o /lib/modules/
mv usb-storage.o /lib/modules/
chmod a+x /lib/modules/scsi_mod.o
chmod a+x /lib/modules/sd_mod.o
chmod a+x /lib/modules/usb-storage.o
cd /
insmod scsi_mod
insmod sd_mod
insmod usb-storage
mknod /dev/sda  b 8 0
mknod /dev/sda1 b 8 1
mknod /dev/sda2 b 8 2
mkdir -m 777 /tmp/apache/htdocs/usbdevice
#mount /dev/sda1 /tmp/apache/htdocs/usbdevice;

## sshd laden und starten
cd /tmp
chmod 600 ssh_host_rsa_key
chmod 600 ssh_host_dsa_key
chmod 755 scp
mv scp /bin/
chmod 755 sshd
chmod +x ssh
mv ssh /usr/bin/
/tmp/sshd -f sshd_config

## madplay
cd /tmp
mv libaudiofile.so.0 /usr/lib/
mv libid3tag.so.0 /usr/lib/
mv libmad.so.0 /usr/lib/
mv libesd.so.0 /usr/lib/
mv madplay /usr/bin/
chmod +x /usr/bin/madplay;
cat << EOF > /usr/bin/madstream
#!/bin/sh
if [ "\$1" != "" ]; then
    echo "Starte Stream \$1";
    wget -q -q -O - "\$1" | madplay -;
else
    echo "Usage: madstream [URL]";
    echo
fi
EOF
chmod +x /usr/bin/madstream;

## apache
cd /tmp
chmod +x /tmp/apache/apache
chmod -R 777 /tmp/apache
chmod 7777 /bin/mount
chmod 7777 /bin/umount
/tmp/apache/apache -f /tmp/apache/conf/apache.conf &
