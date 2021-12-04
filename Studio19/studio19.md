CSE 442S 12/4/2021 Studio 19

## Q1:
Zhuoran Sun
Zhikuan Wei

## Q2:
```
root@jordansunpi:/home/pi# cat /proc/sys/kernel/sched_autogroup_enabled
1
root@jordansunpi:/home/pi# echo 0 > /proc/sys/kernel/sched_autogroup_enabled
root@jordansunpi:/home/pi# cat /proc/sys/kernel/sched_autogroup_enabled
0
```

## Q3:
```
top - 14:00:44 up 42 min,  2 users,  load average: 0.46, 0.70, 0.62
Tasks: 155 total,   3 running, 152 sleeping,   0 stopped,   0 zombie
%Cpu(s): 25.6 us,  0.5 sy,  0.0 ni, 73.1 id,  0.0 wa,  0.0 hi,  0.7 si,  0.0 st
MiB Mem :    926.8 total,    130.0 free,    336.3 used,    460.5 buff/cache
MiB Swap:    100.0 total,     96.2 free,      3.8 used.    499.0 avail Mem 

  PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND                                       
 2113 pi        20   0    1720    312    256 R  99.7   0.0   0:18.48 workload                                      
 1491 pi        20   0   89264  31016  22856 S   4.0   3.3   0:11.23 x-terminal-emul                               
  508 root      20   0  247896  82784  34292 S   2.0   8.7   3:15.34 Xorg                                          
  484 root      20   0   11140   4320   3568 S   1.0   0.5   0:01.81 wpa_supplicant                                
 1505 pi        20   0   96616  38752  25380 S   0.7   4.1   1:34.86 geany                                         
  624 pi        20   0  436944  33892  26492 S   0.3   3.6   0:10.67 lxpanel                                       
 1979 root      20   0       0      0      0 I   0.3   0.0   0:00.21 kworker/0:0-events                            
    1 root      20   0   33768   8132   6524 S   0.0   0.9   0:05.15 systemd                                       
 1590 root      20   0       0      0      0 I   0.3   0.0   0:00.30 kworker/u8:1-brcmf_wq/mmc1:0001:1             
    1 root      20   0   33768   8132   6524 S   0.0   0.9   0:05.15 systemd                                       
    2 root      20   0       0      0      0 S   0.0   0.0   0:00.02 kthreadd                                      
    3 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 rcu_gp                                        
    4 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 rcu_par_gp                                    
    8 root       0 -20       0      0      0 I   0.0   0.0   0:00.00 mm_percpu_wq                                  
   10 root      20   0       0      0      0 I   0.0   0.0   0:00.63 rcu_preempt                                   
   11 root      rt   0       0      0      0 S   0.0   0.0   0:00.01 migration/0                                   
   12 root      20   0       0      0      0 S   0.0   0.0   0:00.00 cpuhp/0 
```
As one can see, workload is always running continuously, taking up about 99.7% of the CPU.
I also used trace-cmd and kernel shark to verified that the workload is always running on 
the designated CPU, CPU 0. See the screenshot `cpu_affinity.png`.

## Q4:
```
top - 13:51:08 up 32 min,  2 users,  load average: 1.41, 0.50, 0.28
Tasks: 164 total,   5 running, 159 sleeping,   0 stopped,   0 zombie
%Cpu(s): 97.5 us,  2.3 sy,  0.0 ni,  0.0 id,  0.0 wa,  0.0 hi,  0.2 si,  0.0 st
MiB Mem :    926.8 total,     95.6 free,    334.9 used,    496.3 buff/cache
MiB Swap:    100.0 total,    100.0 free,      0.0 used.    487.8 avail Mem 

  PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND                                       
 1895 pi        20   0    1720    316    260 R  99.3   0.0   0:19.73 workload                                      
 1896 pi        20   0    1720    320    264 R  99.3   0.0   0:15.51 workload                                      
 1894 pi        20   0    1720    324    268 R  99.0   0.0   0:23.76 workload                                      
 1892 pi        20   0    1720    352    296 R  94.4   0.0   0:30.59 workload                                      
 1491 pi        20   0   88624  30144  22840 S   2.6   3.2   0:07.62 x-terminal
```
Editing using Mousepad is a bit slower and less responsive compared to no background 
tasks. However, browsering using Chromium is a lot slower than having no background tasks.

## Q5:
Without background tasks:
```
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio19 $ time ./dense_mm 300
Generating matrices...
Multiplying matrices...
Multiplication done!

real	0m1.870s
user	0m1.865s
sys	0m0.001s
```
With background tasks:
```
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio19 $ time ./dense_mm 300
Generating matrices...
Multiplying matrices...
Multiplication done!

real	0m3.940s
user	0m1.917s
sys	0m0.001s
```

## Q6:
The user time does not change much with and without background tasks. However, the real time doubles
when there are background tasks compared to without background tasks. The default Linux scheduler
allocates roughly the same amount of time to both processes. I predict the real time to increase as
the number of background tasks that were competing with it to increase while the user time to stay
roughly the same no matter how many background tasks.

## Q7:
```
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio19 $ time sudo nice -n -20 ./dense_mm 300
Generating matrices...
Multiplying matrices...
Multiplication done!

real	0m2.286s
user	0m1.881s
sys	0m0.017s
```
It received 0.822834645669 of the time.

## Q8:
```
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio19 $ time sudo nice -n -10 ./dense_mm 300
Generating matrices...
Multiplying matrices...
Multiplication done!

real	0m2.674s
user	0m2.051s
sys	0m0.012s
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio19 $ time sudo nice -n -5 ./dense_mm 300
Generating matrices...
Multiplying matrices...
Multiplication done!

real	0m3.148s
user	0m2.055s
sys	0m0.027s
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio19 $ time sudo nice -n 0 ./dense_mm 300
Generating matrices...
Multiplying matrices...
Multiplication done!

real	0m4.666s
user	0m2.130s
sys	0m0.000s
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio19 $ time sudo nice -n 5 ./dense_mm 300
Generating matrices...
Multiplying matrices...
Multiplication done!

real	0m9.593s
user	0m2.245s
sys	0m0.018s
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio19 $ time sudo nice -n 10 ./dense_mm 300
Generating matrices...
Multiplying matrices...
Multiplication done!

real	0m23.801s
user	0m2.204s
sys	0m0.042s
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio19 $ time sudo nice -n 19 ./dense_mm 300
Generating matrices...
Multiplying matrices...
Multiplication done!

real	2m43.759s
user	0m2.252s
sys	0m0.017s
```
-10 nice received 0.767015706806 of the time.
-5 nice received 0.652795425667 of the time.
0 nice received 0.456493784826 of the time.
5 nice received 0.234024809757 of the time.
10 nice received 0.0926011512121 of the time.
19 nice received 0.0137519159252 of the time.

## Q9:
```
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio19 $ time ./dense_mm 300
Generating matrices...
Multiplying matrices...
Multiplication done!

real	0m6.428s
user	0m2.091s
sys	0m0.000s
```
It indeed took 3 times the user time as there are 2 tasks competing with it. The user time increased
by a little bit probably because context switches costed time but it didn't change much.
Therefore, it support most of my prediction. There should be an amendment where the user time will
increase slowly with the amount of tasks competing with it due to the frequent context switches.

