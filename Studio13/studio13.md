CSE 442S 11/3/2021 Studio 13

## Q1:
Zhuoran Sun
Zhikuan Wei

## Q2:
```
pi@jordansunpi:~/Documents/CSE422S/writeups/Studio13 $ dmesg
[ 3561.811578] vfs_layer: loading out-of-tree module taints kernel.
[ 3561.811949] Loaded vfs layer module.
[ 3561.812161] Hello from thread vfs_thread.
[ 3561.812177] Filesystem information struct at 6af1a0bd.
[ 3561.812183] Open file information struct at 8e3a8a7b.
[ 3561.812190] Namespace proxy struct at 90e09d68.
[ 3573.785883] Goodbye from thread vfs_thread.
[ 3573.785939] Unloaded vfs layer module.
```

## Q3:
```
pi@jordansunpi:~/Documents/CSE422S/writeups/Studio13 $ dmesg
[ 1213.659877] Loaded vfs layer module.
[ 1213.661127] Hello from thread vfs_thread.
[ 1213.661154] Filesystem information struct at c41496cd.
[ 1213.662473] Open file information struct at 892577ab.
[ 1213.662496] Namespace proxy struct at a083f599.
[ 1213.662510] Root dentry struct at 542ece7c.
[ 1213.662527] PWD dentry struct at 542ece7c.
[ 1213.662538] Name of root (working) directory is /.
```
I think the working directory is the root directory because the pwd will
be the root directory by default.

## Q4:
```
[ 3365.436537] Loaded vfs layer module.
[ 3365.436766] Hello from thread vfs_thread.
[ 3365.436783] Filesystem information struct at c41496cd.
[ 3365.436791] Open file information struct at 892577ab.
[ 3365.436796] Namespace proxy struct at a083f599.
[ 3365.436803] Root dentry struct at 542ece7c.
[ 3365.436810] PWD dentry struct at 542ece7c.
[ 3365.436817] Name of root (working) directory is /.

[ 3365.436831] srv
[ 3365.436836] mnt
[ 3365.436842] lost+found
[ 3365.436849] media
[ 3365.436855] sbin
[ 3365.436861] selinux
[ 3365.436870] lib
[ 3365.436877] bin
[ 3365.436884] opt
[ 3365.436891] root
[ 3365.436900] home
[ 3365.436905] boot
[ 3365.436909] tmp
[ 3365.436915] var
[ 3365.436925] run
[ 3365.436930] sys
[ 3365.436935] proc
[ 3365.436940] etc
[ 3365.436945] usr
[ 3365.436950] dev
pi@jordansunpi:~/Documents/CSE422S/writeups/Studio13 $ ls -l /
total 61
lrwxrwxrwx   1 root root     7 May  7  2021 bin -> usr/bin
drwxr-xr-x   4 root root  4608 Dec 31  1969 boot
drwxr-xr-x  16 root root  3700 Nov  8 09:17 dev
drwxr-xr-x 120 root root  4096 Oct 11 01:10 etc
drwxr-xr-x   3 root root  4096 May  7  2021 home
lrwxrwxrwx   1 root root     7 May  7  2021 lib -> usr/lib
drwx------   2 root root 16384 May  7  2021 lost+found
drwxr-xr-x   2 root root  4096 May  7  2021 media
drwxr-xr-x   2 root root  4096 May  7  2021 mnt
drwxr-xr-x   6 root root  4096 May  7  2021 opt
dr-xr-xr-x 168 root root     0 Dec 31  1969 proc
drwx------   4 root root  4096 Oct 10 16:15 root
drwxr-xr-x  28 root root   820 Nov  8 09:17 run
lrwxrwxrwx   1 root root     8 May  7  2021 sbin -> usr/sbin
drwxr-xr-x   2 root root  4096 May  7  2021 srv
dr-xr-xr-x  12 root root     0 Dec 31  1969 sys
drwxrwxrwt  15 root root  4096 Nov  8 10:09 tmp
drwxr-xr-x  11 root root  4096 May  7  2021 usr
drwxr-xr-x  11 root root  4096 May  7  2021 var
```
The output of our module and `ls -l /` are the same except for the fact that
they are listed in different orders, and `ls` also shows the directories
that proxy directories are pointed to.

## Q5:
```
pi@jordansunpi:~/Documents/CSE422S/writeups/Studio13 $ dmesg
[ 4580.524825] Loaded vfs layer module.
[ 4580.525036] Hello from thread vfs_thread.
[ 4580.525056] Filesystem information struct at c41496cd.
[ 4580.525066] Open file information struct at 892577ab.
[ 4580.525074] Namespace proxy struct at a083f599.
[ 4580.525084] Root dentry struct at 542ece7c.
[ 4580.525094] PWD dentry struct at 542ece7c.
[ 4580.525101] Name of root (working) directory is /.
[ 4580.525111] .flatpak-info
[ 4580.525118] srv
[ 4580.525126] mnt
[ 4580.525134] lost+found
[ 4580.525143] media
[ 4580.525151] sbin
[ 4580.525159] selinux
[ 4580.525166] lib
[ 4580.525174] bin
[ 4580.525181] boot
[ 4580.525189] run
[ 4580.525196] sys
[ 4580.525203] proc
[ 4580.525211] dev
[ 4597.782779] Goodbye from thread vfs_thread.
[ 4597.782913] Unloaded vfs layer module.
```
The output is different from `ls -l /` in that proxy directories and empty directories
are not printed.

## Q6:
Kernel Module:
```
pi@jordansunpi:~/Documents/CSE422S/writeups/Studio13 $ sudo insmod vfs_layer.ko
pi@jordansunpi:~/Documents/CSE422S/writeups/Studio13 $ dmesg
[ 8747.930478] Loaded vfs layer module.
[ 8747.930614] Hello from thread vfs_thread.
[ 8747.930626] Filesystem information struct at c41496cd.
[ 8747.930634] Open file information struct at 892577ab.
[ 8747.930640] Namespace proxy struct at a083f599.
[ 8747.930647] Root dentry struct at 542ece7c.
[ 8747.930653] PWD dentry struct at 542ece7c.
[ 8747.930658] Name of root (working) directory is /.
[ 8747.930665] .flatpak-info
[ 8747.930670] sbin
[ 8747.930675] selinux
[ 8747.930680] lib
[ 8747.930685] bin
[ 8747.930690] boot
[ 8747.930695] run
[ 8747.930700] sys
[ 8747.930705] proc
[ 8747.930710] dev
pi@jordansunpi:~/Documents/CSE422S/writeups/Studio13 $ sudo rmmod vfs_layer 
pi@jordansunpi:~/Documents/CSE422S/writeups/Studio13 $ sudo insmod vfs_layer.ko nr=3005
pi@jordansunpi:~/Documents/CSE422S/writeups/Studio13 $ sudo rmmod vfs_layer 
pi@jordansunpi:~/Documents/CSE422S/writeups/Studio13 $ dmesg
[ 8747.930478] Loaded vfs layer module.
[ 8747.930614] Hello from thread vfs_thread.
[ 8747.930626] Filesystem information struct at c41496cd.
[ 8747.930634] Open file information struct at 892577ab.
[ 8747.930640] Namespace proxy struct at a083f599.
[ 8747.930647] Root dentry struct at 542ece7c.
[ 8747.930653] PWD dentry struct at 542ece7c.
[ 8747.930658] Name of root (working) directory is /.
[ 8747.930665] .flatpak-info
[ 8747.930670] sbin
[ 8747.930675] selinux
[ 8747.930680] lib
[ 8747.930685] bin
[ 8747.930690] boot
[ 8747.930695] run
[ 8747.930700] sys
[ 8747.930705] proc
[ 8747.930710] dev
[ 8756.603407] Goodbye from thread vfs_thread.
[ 8756.603454] Unloaded vfs layer module.
[ 8766.102377] Loaded vfs layer module.
[ 8766.103973] Hello from thread vfs_pid_thread.
[ 8775.089162] Goodbye from thread vfs_pid_thread.
[ 8775.089243] Unloaded vfs layer module.
```
Simple Program:
```
pi@jordansunpi:~/Documents/CSE422S/writeups/Studio13 $ ./test.o
PID: 3005
Hi from kernel thread.
```
Yes, the module was able to successfully write its message to the target process's
standard output.