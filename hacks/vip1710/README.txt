<pre>
File: README.txt ------------------------------------------------------------

(c) 2010, Hans-Werner Hilse <hilse@web.de>, http://twitter.com/hilse
All text stuff licenced under CC-BY-SA:
http://creativecommons.org/licenses/by-sa/2.0/

Programs published here are under various licenses. If those licenses require
to publish source code, the sources will either be provided here in the
download section or - this generally means I left them unmodified from their
upstream versions - I'll happily provide you upon request. Just drop me a
mail (see email address above).

Everything here is private and non-commercial. Some of the product and
company descriptions here may contain trademarks. I do not claim any
of these, they belong to their respective owners.

If you were directed directly to the README.txt, you will be missing the
download section. Simply go to http://hilses.de/vip1710 then!
         - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


Hi there!

This is my private file store for things regarding hacking a VOD set top box,
to be more specific: This is all about the VIP 1710, a product by Kreatel
and/or Motorola.

More information about the device might be found here:
http://www.mikrocontroller.net/articles/MOTOROLA_VIP1710 (german)

There's a discussion thread here:
http://www.mikrocontroller.net/topic/pollin-motorola-vip1710


As I got to buy a pair of these for 5 EUR each, I was eager to start hacking.
Now that I finally found some time, I'll share my results here for those
who are interested.

All of this will change when I learn more about this device. But after some
first inspections, I already got very far, I think.


Contents:
=========

1. Make it boot your way
1.1. Preface
1.2. Boot procedure
1.3. getsettingsvalue
1.4. A warning
1.5. The shellcode
1.6. Using the stuff
1.7. Boot flow
1.8. C&P: Making your device USB-bootable (kind of...)
1.9. Overlaying the "real" settings2.xml

2. Taming halserver

3. NFS mounts

4. The native build environment
4.1. Installing
4.2. Using it
4.3. Software provided

5. A LIRC driver for the UIRT
5.1. Driver info
5.2. Usage

6. Notes on the platform

X. Misc stuff

DISCLAIMER / HAFTUNGSAUSSCHLUSS

DOWNLOAD SECTION

                ------------------------------------------


1. Make it boot your way

1.1. Preface

When I started playing with the box, I soon realized that I got one of those
which have onboard flash to store its firmware. Nice, I thought, I can change
it to make it what I want it to be. But that turned out to be (almost!)
impossible, since the box uses some bootloader techniques to ensure a
cryptographic signature for the firmware.

So the flash cannot be touched... Flash is entirely secured by the Bootloader.
Well, not entirely... One small village of indomitable Gauls -- err, no,
different story. What I wanted to say is: there is some space for runtime
configuration data which is preserved and _is_ user-accessible:

The directories /flash (JFFS2) and /flash2.

1.2. Boot procedure

The device uses a classic busybox environment and the init it provides.
This is reading /etc/inittab, which basically does this:

(1) for system initialization, it starts /etc/rc.sysinit
(2) it starts a respawning /bin/sh, I guess on the serial console
(3) it starts the telnet daemon
(4) after system initialization, /usr/bin/start_platform.sh is run

1.3. getsettingsvalue

When examining (1) and therefore /etc/rc.sysinit, there is in fact one single
line which reads configuration from the aforementioned persistent config space:


	# Set time zone
	timezone=""
	if [ -f /flash/settings2.xml ]; then
	  timezone=$(getsettingsvalue /flash/settings2.xml config.timezone)
	fi
	if [ -z "$timezone" ]; then
	  echo "No time zone setting, using UTC"
	  timezone="UTC"
	else
	  echo "Setting time zone $timezone"
	  ln -sf /usr/share/zoneinfo/$timezone /etc/localtime
	fi


Note the line where it's calling "getsettingsvalue".
This is where we will get into things. Soon.

I played a lot around with this binary, peeked and poked, and finally found out
it is some C code, obviously written in a hurry and - lucky us - it's buggy. It
is prone to stack buffer overflows which brings us in the situation to smash
its stack by handing over a carefully crafted /flash/settings2.xml.

Reading such a file will make getsettingsvalue overwrite a return address which
it stored on the stack. After executing its tasks, it will read the address
back and jump there. This is where we get in the way.

1.4. A warning

Please be warned: The following describes modifications that allow for a
permanent change in the boot process of the device. As this happens at an early
stage, it is possible that you make modifications which will effectively brick
the device. This would be the case when you manage to interrupt the boot flow
permanently. This happens easily: Just have anything waiting for input and
that's it. So be very, very careful and _always_ test your scripts before
making them permanent!

1.5. The shellcode

I wrote some shellcode (I learned about MIPS assembler language when I was
examining getsettingsvalue) and crafted a settings2.xml file that does basically:

	execve("/bin/sh", [ "/bin/sh", "/flash2/myrc", NULL ], NULL);

And while it was a ride through hell debugging this stuff on the device (cache
coherency issues were everywhere), I now have a version that is working
reliably. You can find it below among the downloads.

BTW, you can try to test it before bringing it into place. Just download it to
your box and execute "getsettingsvalue /path-to-downloaded-settings.xml test".
It should execute /flash2/myrc then. But you might as well get a signaled abort,
signal would then be SIGILL, i.e. illegal instruction. This is due to cache
coherency issues. It has a tendency to break when the box is otherwise idle.
If you really want to test it out, make the box busy - it's working best when
the box is doing lots of context switches (i.e. userland->kernel->userland)
since this blows out the cache lines more easily. Luckily, upon boot there is
a lot of activity and the exploit has been 100% reliable for me there.

1.6. Using the stuff

In order to use it, just copy it to /flash/settings2.xml. You might need to
delete an existing file and might want to backup it beforehand.

It is of bigger size than nominally fits into /flash, but this is actually not
true because /flash is on a compressed file system. So it should fit. I don't
use /flash for anything else, so it's just this file there.

I also have a corresponding /flash2/myrc shell script. This script is being
called by the shellcode and can do anything I'd like to.

Below you can find an example script that I will use for now for debugging.
It has some dependencies: You need to put kernel modules scsi_mod.o, sd_mod.o
and usb-storage.o into /flash2/modules.

It will load them at boot time, wait 5 seconds for devices to appear (usb
storage devices in case you did not realize from the modules). If there is a
storage device, it will try to mount the first partition to /mnt. It will then
look for a file named "plugrc" and if it exists, /bin/sh is executed with this
script.

This way you can use usb storage devices as a kind of "boot floppy". This
allows to have a flexible boot process, have application-specific storage
devices and boot accordingly. If there's no usb storage device present, the
boot will just continue as usual (so the box will do DHCP and offer telnet
access).

1.7. Boot flow

After execution of /flash2/myrc, /etc/rc.sysinit will just continue the usual
boot process. So you can use this mechanism to make some adaptions to the ram
based root filesystem before the whole kaboodle kicks in (halserver, X server,
browser, ...) and then just let it continue. On the other hand, you can
permanently intercept the boot process by not returning from your script. But
be very, very careful that you keep access to the device, otherwise it will be
bricked. This is why I implemented the "usb storage device way" outlined above.

1.8. C&P: Making your device USB-bootable (kind of...)

The following should make you device usb-bootable (i.e. it will exec 
<devroot>/plugrc with /bin/sh in early boot process), carefully check if errors
are spewn out and stop by the earliest sign that something might be wrong
(actually compare those MD5SUMs!):

---snip

$ telnet <ip-of-your-box>


BusyBox v1.01 (2007.01.11-16:43+0000) Built-in shell (ash)
Enter 'help' for a list of built-in commands.


/ # wget http://hilses.de/vip1710/usb-storage-modules.tar
Connecting to hilses.de[62.75.159.166]:80
usb-storage-modules. 100% |**********************************|   187 KB    00:00 ETA
/ # wget http://hilses.de/vip1710/settings-xml--stack-overflow-exploit.bin
Connecting to hilses.de[62.75.159.166]:80
settings-xml--stack- 100% |**********************************|   128 KB    00:00 ETA
/ # wget http://hilses.de/vip1710/myrc                                    
Connecting to hilses.de[62.75.159.166]:80
myrc                 100% |**********************************|   1059      00:00 ETA
/ # md5sum usb-storage-modules.tar 
c9a1422e8b1ad4cd1ac8e8af0732f7d2  usb-storage-modules.tar
/ # md5sum settings-xml--stack-overflow-exploit.bin 
84d6f170d81c4633e6e5623aa986bd3b  settings-xml--stack-overflow-exploit.bin
/ # md5sum myrc 
62dbb9bf6feabe701a8c3785dcbe74bf  myrc
/ # tar -xv -f usb-storage-modules.tar -C /flash2 
modules
modules/scsi_mod.o
modules/sd_mod.o
modules/usb-storage.o
/ # cp myrc /flash2/
/ # mkdir /flash2/flash-overlay
/ # cp /flash/settings2.xml /flash2/flash-overlay/settings2.xml
/ # cp settings-xml--stack-overflow-exploit.bin flash/settings2.xml 
/ # sync
/ #

---snip

Give the box a bit of grace time to really make sure changes are written to
flash. Reset the box, start without any usb storage media plugged in. If you
see a 0-sized file /yes-sir-we-can-boogie then everything went well!

1.9. Overlaying the "real" settings2.xml

I've added a feature to myrc (the version I provide below) that allows to
have the exploited settings2.xml in place during boot up and then having
/flash point to a different place afterwards. This way, you can now
create a directory /flash2/flash-overlay which is bind-mounted to /flash
by the myrc script. You can just put your old settings2.xml into this
directory (i.e. /flash2/flash-overlay) and toish & Co will continue to
work just fine.

I've added the relevant commands to the C&P section in section 1.8. (the
"mkdir /flash2/flash-overlay" and the following "cp")


                ------------------------------------------

2. Taming halserver

This section is a bit shorter than the first :-)
When playing with your own hand-tailored userland, you will stumble upon some
missing bits and pieces.
Some of them, first and foremost: audio support, are provided by the

/usr/bin/halserver

program. It communicates with the Xilleon special device /dev/atyx2200 and
issues the proper commands. We will still have to reverse engineer these.

In the meantime, we can use halserver which knows about the special language of
the mentioned platform device - and provides some standard interfaces for us.
For the technically interested: it uses the "fusion" device, which is a technique
that has its origins at the designers of the directfb gfx lib. The "fusion"
device is a kernel-userland interface and can create further devices from kernel
space which you can then drive from userspace (i.e. userspace implementations
of read(), write(), ioctl(), open(), close()...). It does this for providing 
OSS compatible (except they don't like mono-channel audio) devices for sound
output, implementing the functionality in userspace. Basically it's a rather
ineffective wrapper that goes through at least one unnecessary userland->kernel->
userland step. But that's what we have until we know better how to drive
/dev/atyx2200 on our own behalf.

Unfortunately, halserver also drives the watchdog timer. I say "unfortunately",
because halserver expects a kind of "heartbeat" signal from the userland software
stack supervisor processes. But since this is about _not_ starting the full
userland, there will be no such heartbeat.

In the end, the easiest and quickest (and rather dirty) solution I came up with
was just overriding some libc functions by preloading my own library. In it, I
catch open() and ioctl() calls for the watchdog device and just drop them. This
way, halserver doesn't mess with the watchdog.

You find the library, prevent-halserver-watchdog.so, among the downloads below.
Load it onto your box, make sure it's executable and then start halserver, e.g.
in a script, like this:

   touch /tmp/halsocketname
   LD_PRELOAD=/path/to/prevent-halserver-watchdog.so /usr/bin/halserver -s /tmp/halsocketname &

You might also be interested in how to stop the currently running halserver
which got started during the boot process (if you didn't modify that).
In short, you can use this after logging in:

   killall -9 halserver procman; watchdog -t 5 /dev/misc/watchdog

This will kill halserver and procman (which also triggers reboots in my 
experience) and immediately start up a watchdog userland daemon which serves
the watchdog device.

That's it!


                ------------------------------------------


3. NFS mounts

In the download section you can also find a package nfs.tar.gz which contains
compiled modules and daemons (WARNING: a bit outdated version 1.0.10 of
nfs-utils! Will probably have its security flaws!) to get you going with
mounting NFS exports on the device. It has an initialization script which
loads the modules and starts the daemons. You should get the idea by reading
it. It's in /bin/start-nfs.sh.

To test it (on the device):

---snip

/ # wget http://hilses.de/vip1710/nfs.tar.gz
[...]
/ # tar xvzf nfs.tar.gz
[...]
/ # start-nfs.sh
[...]
/ # mkdir /nfs-mount
/ # mount -t nfs HOST:/export /nfs-mount

---snip



                ------------------------------------------


4. The native build environment

I've finished the boring work of building a native toolchain, running on the
device itself. You can find it in the download section.

4.1. Installing

To use the toolchain, you must use an external usb storage medium or use NFS,
e.g. using the kernel modules provided on this page. I will only explain the
usage of an external usb storage medium here, because I think you can figure
things out for NFS yourself if you managed to set it up :-) Just a short hint
for NFS users: check the "exports" man page for "root squashing" if you are
not yet used to the innards of NFS.

For those of you that decide to use an external USB storage medium, I will
describe the basic steps to get the native build environment running.
BTW, running from USB media has been faster than NFS for me.

First, select the media carefully. I recommend a (spinning) hard disk, not
flash storage. As the latter will certainly work, too, you may be wearing out
the medium if you do lots and lots of writes (and you are a hardcore developer,
right?). Plus, if you want to use swap, you will want this to be on a hard
disk, not flash (for the same reason, basically).

Note: the extracted build environment alone needs about 500 MByte of space.

Then there's bad news: You will need a partition formatted for this. So
what file system to chose? Personally, I went with ext2. This is easy, since
the kernel running on the box already has ext2 support. Another option would
be jfs. A jfs kernel module (to be put in /lib/modules and then insmod'ed
with "insmod jfs") is provided in the download section. Use this if you are
a lazy plug-puller and always forget to remount the drive read-only and sync
before pulling DC - you will be thankful for a logging file system.

Obviously, you will need to format the drive either from a running Linux
system (you could use a live CD, perhaps?) or with some obscure and unknown-
to-the-author other tools on your OS of choice.

Then you simple un'tar the archive file provided in the download section
onto the new filesystem (which you have mounted):

# mount /dev/sdb1 /mnt
# cd /mnt
# tar xvJf /home/user/downloads/vip1710-native-build-environment.tar.xz

That's it.

Note that if you use a harddisk, you should use the opportunity and create
a swap partition. OTOH, you can always create a swapfile later. How to use
a swap partition or file is beyond the scope of this document. But inform
yourself about "mkswap", "swapon", "swapoff" commands (and "losetup" if you
intend to use a swap file).

4.2. Using it

First: The build environment contains a few files that integrate with the
"myrc" provided on this page (also read its documentation above). You do not
have to use this "myrc", though. But if you do, here are some notes:
it will use the infrastructure consisting of plugrc{,.d,.conf} files. They
will make the box start up with just a minimal set of binaries running.
Also they will set the IP address according to the configuration in
"plugrc.conf". Make sure to give it a look and edit it to your needs!

Regardless of whether you use my "myrc", here is how it works:

$ telnet <box ip>
[...]

then, if you use my myrc, /mnt already is a read-only mount, you'll do:

# mount -o remount,rw,noatime /mnt

if OTOH you don't use my myrc, you might need to load jfs modules, then,
for jfs and ext2, do (adapt device/partition if needed):

# mount -o noatime,rw /dev/scsi/host0/bus0/target0/lun0/part1 /mnt

and then all you have to do is this

# /mnt/chroot.sh

You will then be in a chroot environment which provides you with gcc&Co.
Note that GNU Screen is provided for creating sessions which keep running
even if you log out of the box. After logging in again later, just call
the /mnt/chroot.sh script and then re-attach the Screen session. Read
about GNU Screen if you don't know it already - it helps in a lot of
cases.

Now "./configure; make; make install" as you like!

4.3. Software provided

Here is a list of software that is provided within the build environment.
I will add links to the project pages. Note that not a single source
was patched (all my work was starting the compiler, over and over again),
so if you're interested in the source, look up the project. Links on this
page is coming soon. If in doubt, look at GNU: most of the tools are GNU
tools. (Please note the comment on top of this page: If you want, you can
always drop me a mail and I can provide you with the source code!)

So here's the list, I've highlighted some things I considered important:
	apr-1.4.2
	apr-util-1.3.9
*	autoconf-2.66
*	automake-1.9.6
*	bash-4.1
	bc-1.06
*	binutils-2.20.1
	bison-2.4.2
	bzip2-1.0.5
*	ccache-3.0.1
	coreutils-8.5
	cpio-2.11
	curl-7.21.0
*	cvs-1.11.23
	diffutils-3.0
	ed-1.4
	expat-2.0.1
	findutils-4.2.29
	flex-2.5.35
	gawk-3.1.8
*	gcc-3.4.6
*	gdb-7.1
	gettext-0.18.1.1
*	git-1.7.2.1
	gmp-5.0.1
	grep-2.6.3
	gzip-1.4
	inetutils-1.8
	iproute2-2.6.34
*	iptables-1.4.8
*	jfsutils-1.1.14
	jpeg-8b
	less-418
	libelf-0.8.13
	libtool-2.2.10
	m4-1.4.9
*	make-3.81
	ncurses-5.7
*	openssh-5.5p1
*	openssl-1.0.0a
	patch-2.6.1
*	perl-5.12.1
	pkg-config-0.25
*	Python-2.7
	readline-6.1
*	scons-1.3.1
*	screen-4.0.3
*	SDL-1.2.14
	serf-0.6.1
	sharutils-4.9
	sqlite-3.7.0
*	subversion-1.6.12
	tar-1.23
	termcap-1.3.1
	termutils-2.0
	time-1.7
	unzip60
	util-linux-ng-2.18
*	vim72
	wget-1.12
	xz-4.999.9beta

Now, I hope I did not forget about something. Please notify me if you
think something is missing in this list!

Please respect the licenses of the software, included in the tarball.

Please note I used unmodified upstream sources. I also provide a download
mirror for them here: http://gpl.hilses.de/vip1710/upstream-mirrored but I
would suggest to use the current upstream sources. Include files and libraries
are contained within the build environment tarball anyway.


                ------------------------------------------


5. A LIRC driver for the UIRT

The VIP1710 has a built-in Infrared receiver. The ATI Xilleon platform
contains a "UIRT", a Universal Infrared Receiver/Transmitter. There are
ATI Xilleon platforms which have more of them, however. I do not have
access to such a platform, so the driver provided here will most probably
be broken on such devices. If you want to adapt the driver to such a
platform, feel free to contact me for discussing what would be needed.

The UIRT also provides means for transmitting IR, as the name indicates.
It would not be too hard to implement the transmission in the driver,
however I did not do that yet. So the driver is currently *RX only*!
This has a simple reason: The VIP1710, at least the model I have here,
does not provide a IR transmission diode. I did not yet investigate
whether contacts for soldering one are provided. Another possibility
would possibly be to use the status LED for this (of course we would
have to de-solder it and solder in a IR diode). However, this might never
work, so I chose to not bother yet with TX.

5.1. Driver info

As the sources for the kernel are currently Linux 2.4, I had to use
an outdated version of LIRC. So it's version 0.8.2, which is said to be
the last version which officially supported Linux 2.4.

The driver uses the lirc_dev framework, so you will have to compile
this driver too (or use the ready compiled module).

Then there's driver/lirc_xilleon which contains the Xilleon-specific
code. I followed the steps taken in the lirc_serial driver, but since
we will only do RX for now and the UIRT does RLE encoding of pulses and
spaces for us, it is much simpler.

The driver is interrupt-triggered. It will generate one interrupt per
N samples, where N is configurable. Also it knows a means to timeout,
but that isn't really used for now. You can however set those as
module parameters.

You can also set the duration of a single sample interval (note that
the UIRT returns RLE encoded values anyway, so it defaults to a fine
resolution of 50us currently).

Note that the source is provided as a patch against LIRC 0.8.2
(which is also provided in the ZIP, compressed with XZ). You will
need to run "aclocal; autoconf; automake --add-missing" in order
to rectify the autotools scripts (so you'll need autoconf/automake!).
In order to use it with libtools >= 2.0 I also had to switch
"libtool" and "ltmain.sh" to the system versions.

5.2. Usage

Note that in order to use this infrared driver, the vendor driver
must be unloaded! To do this, "rmmod 08ir" (and most probably you
will have no use for the "07kb" driver, either).

Install the modules and binaries, e.g. simply by downloading the
.tar.bz2 file below and un-tar it in the tmpfs root of the device.

Then just insmod lirc_dev and lirc_xilleon!

Use irrecord to learn codes, mode2 to print raw mode2 values,
lircd as the daemon etc. See the many HOWTOs on the Web.
For a fast check if it works, simply run "mode2 -d /dev/lirc/0" and
press a button on your IR remote control of choice.

lirc_xilleon also has module parameters:
debug:
	set this to 1 to generate lots of debug output
sample_div:
	this is the sample duration in N/48 us. Defaults 
	to 2400, which is 50us.
timeout:
	I could still not make sense of the unit this is in.
	Defaults to 1000 which seems to work here...
fifo_threshold:
	How many RLE samples in the FIFO to trigger interrupt,
	defaults to 4
                
Play with them as you like and have fun!


                ------------------------------------------


6. Notes on the platform

In this section I will collect some notes on the platform. Hopefully this
will help others to get started with hacking on it.

6.1. Streaming with the userland tools

The "toish" shell can invoke some Ipc calls to the vendor userland software
stack. For me the most important one is probably

  toish uri LoadUri <URI> <MIME-Type>

Streaming needs a running X server, halserver, streamer, mediamanager and
maybe others.

Possible URIs are
- http://...  gets TS or PS via HTTP
- file://...  gets TS or PS (try name appendix ".ts", ".mpg") from file
              or from a named pipe (!)
- tcp://<IP>:<PORT> gets TS or PES(?) or PS(?) from a raw TCP connection
              will auto-retry for a while when stream is cut
- udp://<IP>:<PORT> listens for UDP packages containing TS data on a
              certain <IP>/<PORT>
- igmp://<IP>:<PORT> does probably the same. Don't know yet if it properly
              registers a IGMP multicast address, though. But listening
              on multicast addresses works.
- rtsp://<IP>:<PORT> did not yet get it to work, but this will ask a
             RTSP server for an RTP stream
- more...?

Possible Mime-Types are
- video/mpeg is tested, works for MPEG data (TS/PS/PES)
- maybe others, there's a list of MIME-Types registered in tvvod by
  streamer, it's in /usr/application/vod, I think (TODO: verify).


                ------------------------------------------


X. Misc stuff

Audio:
(note that you will want to have halserver running, see above section 2!)

- There's a compiled esound for you, i.e. esd & co. It's from unmodified
  0.2.41.

- There's a compiled sox for you, i.e. sox, play, rec. It's from unmodified
  upstream version v14.3.1

Filesystem:

- There's a JFS file system kernel module for you. Note that it's untested,
  if it works for you, I'd like to hear about it!

- Upon a request, I compiled a version of the CIFS network filesystem for
  accessing Samba servers. It's a bit outdated (1.20c backport for Linux 2.4),
  but it works for me. In the archive, a version of mount.cifs is provided,
  too.

- I packaged Samba 3.5.4 and nfs-utils 1.2.2 as a convenience package.
  It's prepared to be used together with the settings2.xml exploit and
  will start Samba and NFS upon boot. Note that the samba executables are
  rather big. The package is called nas-vip1710.zip, and to use it, copy
  the contents of the contained "nas-vip1710" directory to the root of a vfat
  formatted partition on a USB hard disk (or flash stick).

Gaming:

- There's a joydev.o kernel module for you.

- gnuboy-1.0.3.directfb.tar.xz contains gnuboy-1.0.3 sources which are
  modified to also support directfb output and esound output. They contain
  a readily made dfbesdgnuboy executable which is capable of running on
  the VIP1710. Make sure you have halserver and esound running (see section
  on halserver above and have a look at the provided esound archive in the
  download section). Also you need to use the joydev kernel module and
  insmod it. Then run

  ./dfbesdgnuboy --joy_device=/dev/input/js0 --scale=3 <romfile>

  I think it's still a bit too slow due to PAL refresh rates (the box only
  does 25 (!) fps when it comes to waiting for vertical retraces). When running,
  it takes about 50% CPU.

- For example purposes, I also built a "USB stick distro" of GnuBoy. You
  can download it below in the download section. Also its sources are provided,
  which you might be interested in if you need e.g. a working DFBTerm for
  your box (which I also made USB-gamepad ready!).

  For installing this mini-distro, you need to put only two files from the
  ZIP onto a USB stick or harddrive, which may be VFAT or ext2 formatted:
  "plugrc" and "hwsdist-root.tar.bz2". Also, you will want to create a
  directory for your ROMs, which must be named "roms-gnuboy". Put all your
  ROMs in there.

  The distro is meant to be used with the myrc script provided here.
  Just plug in a USB Hub with your gamepad and the prepared USB stick/HD and
  power on the device. It should eventually present a menu which you should
  be able to navigate using your game pad. You should still be able to log
  in using telnet, but it will not start the normal STB environment.

  If you're not using my myrc script, you should anyway be able to use the 
  provided programs by mounting the stick to /mnt, then run /mnt/plugrc and
  after that, run /usr/bin/start_platform.sh (which will be replaced by
  plugrc)

Various:

- I started writing a small app for toggling LED status which will possibly
  in the future also provide some other functionality. Thus the name
  "kboxctl". You can find it compiled and its C source in the archive
  kboxctl.tar.bz2

- Also, I started writing a VNC server for DirectFB. It is currently tailored
  for a 720x576 @ ARGB32 DirectFB framebuffer. It does not do any
  double-buffering and comes with a hack that will make all pixels transparent
  if a full black frame is sent by the server. This allows to use it as
  OSD display for VDR with the ffnetdev-plugin.
  You invoke it like this: "vipvnc server:port".
  It does not yet support input, maybe it never will. Thus it is only capable
  of displaying!

  For use with the ffnetdev-plugin, you should for now use the provided LIRC
  solution and use LIRC's client/server functionality. Works great for me.
  Then you can start the stream with
    toish uri LoadUri tcp://<VDR-IP>:20002 video/mpeg
  and fire up vipvnc for the OSD with
    vipvnc <VDR-IP>:20001
  However, the ffnetdev-plugin is old and it has been very unstable for me.
  Might be the network here, too, I'm not sure yet and this is definately
  an experimental WIP project which I might never finish.

  Oh, and one thing: I had to increase the amount of video data buffered
  by the streamer process, it can be configured in /etc/streamer_conf.xml
  and 500ms worked for me. (this applies to the video data, not VNC)

- I finished an early try to implement a plugin for VDR to have VDR run
  on the box itself. Combined with streamdev-client, this allows to watch
  TV, using some other VDR on the LAN to provide the actual DVB data
  (using streamdev-server).
  The plugin will use DirectFB to provide a full-color OSD, with perfect
  Alpha blending.
  It has been updated since initial publication and now supports playback
  of recordings, too. Said that, fast-forward and rewind is not supported yet,
  which means you have to use jumping instead.
  Setting of volume and setting of the Scart output's wide-screen signalling
  is supported now, too.

  You find a full minidistro (intended for use on USB media and with the
  settings2.xml hack together with the myrc file I provide) among the
  downloads below. Extract the ZIP and copy all files in the distro folder
  to the root of your USB media FS. Have it plugged in when booting the box.

  The Minidistro will load LIRC modules for the UIRT, but you need to set
  up a remote first. In order to do that, log in via telnet and do

    irrecord -d /dev/lirc/0 /mnt/etc/lirc.conf

  and follow the steps indicated. Then edit plugrc on the USB media and
  comment in the autostart procedure in the last line (if you want to
  autostart it, for testing issue the commands manually).

  You will most probably use this with the included (and automatically loaded)
  streamdev-client plugin. So you will have to configure this and copy a
  channels.conf file in /etc/vdr on the USB media. Copying over the
  channels.conf from the VDR that has the streamdev-server running should be
  sufficient. You can configure the IP of the remote server in the plugin
  settings within VDR.

  You can find the source for the plugin in the directory
  vdr-vip1710-minidistro.src in the download section below.

- For figuring out what the box' vendor software stack does, I wrote a
  small utility which can dump unix domain socket communication. This will
  allow you to see the messages the individual programs are passing to
  each other (they do IPC by these means). You can find it in the
  sockdump package, and its source is here: http://hilses.de/sockdump

- To issue IPC calls to the vendor software stack (or receive IPC calls from
  the vendor software stack) I wrote a C library, libvipipcstack. It's now
  in the folder libvipipcstack below and comes with a documentation on the
  IPC stack.


                ------------------------------------------


DISCLAIMER:

PLEASE NOTE THAT YOU MAY USE THESE FILES FOR WHATEVER YOU THING MIGHT FIT, BUT
I DO NOT GUARANTEE FOR ANYTHING. IF YOUR BOX WENT AFLAME, EXPLODED, BURNED YOUR
HOUSE DOWN: I WARNED YOU. THIS IS ALL DONE WITH GOOD WILL AND I DO NOT INTEND
EVIL, BUT I DO NOT GIVE ANY GUARANTEES.


HAFTUNGSAUSSCHLUSS:

DIE FOLGENDEN DATEIEN WERDEN VON MIR BEREITGESTELLT UND DUERFEN VERWENDET
WERDEN WOZU AUCH IMMER ES BELIEBT. ICH GARANTIERE ABER FUER NICHTS UND
SCHLIESSE AUSDRUECKLICH JEDE HAFTUNG AUS, SO WEIT MOEGLICH. DIESE DATEIEN
WURDEN NACH BESTEM WISSEN UND GEWISSEN ERSTELLT UND BEABSICHTIGEN KEINERLEI
SCHAEDIGUNG.
</pre>
