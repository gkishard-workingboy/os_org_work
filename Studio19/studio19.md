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