Zhikuan Wei

Module Design
Declare two unsigned long named log_sec and log_sec. In order to make two parameters to be 
invisible to the filesystem, set the permissions to be 0, which disables the entry altogether.
By setting the default value of sec to 1 and nsec to 0 respectively, after module is loaded,
it will reschedule every second.

Timer Design and Evaluaion
Timer design first need two variable, one for struct hrtimer and another for time interval.
When the module is loaded, it enters the init funciton, firstly use log_sec and log_nsec to
initialize the interval. Secondly, we initialize the timer and specify the expiry by interval.
After that, we link the field function of timer to our tick function, which will be called once 
the timer expires, and use it to extend the expiry of timer, and print out "tick..." in syslog.
Finally, we let the exit function of module to terminate the timer.

System log fragments:
default value:
[Oct10 14:23] lab1 module initialized, with log_sec:1 and log_nsec:0
[  +1.000050] tick...
[  +1.000002] tick...
[  +1.000002] tick...
[  +1.000006] tick...
[  +1.000004] tick...
[  +1.000002] tick...
[  +1.000004] tick...
[  +1.000006] tick...
[  +1.000003] tick...
[  +0.567619] lab1 module is being unloaded

specified value:
[Oct10 14:28] lab1 module initialized, with log_sec:0 and log_nsec:100000000.
[  +0.100044] tick...
[  +0.100000] tick...
[  +0.100001] tick...
[  +0.100001] tick...
[  +0.099999] tick...
[  +0.100000] tick...
[  +0.100002] tick...
[  +0.100000] tick...
[  +0.100005] tick...

Comments:
As we can see, the biggest variation happens in ther first line. That's because other operation 
inside the init function cost some time. And for general, the variation time is no more than 
6 microseconds.

Thread Design and Evaluation
For Thread Design, we add a new static task_struct pointer variable, and once module is loaded, 
we create a new kernel thread and assign that to the pointer. And we also prepare a new thread 
function for that thread to run in loop. Inside every loop, we firstly print out useful debug info, 
then we let thread be interruptible and yield itslef. And we add a new line in timer to wake up 
the thread. After being invoked and before to start a new loop, we check whether the thread need 
to stop, if so we just jump out the loop and return 0. Finally, we let the exit function to send 
stop signal to the thread, then we can make sure every time the module is unloaded, the kernel 
thread will stop running.

System log fragment:
[Oct10 15:46] lab1 module initialized, with log_sec:1 and log_nsec:0
[  +0.000332] now we are working!!!
[  +0.000022] current status nvcsw 1 and nivcsw 0
[  +0.999979] tick...
[  +0.000027] current status nvcsw 2 and nivcsw 0
[  +0.999985] tick...
[  +0.000026] current status nvcsw 3 and nivcsw 0
[  +0.999972] tick...
[  +0.000021] current status nvcsw 4 and nivcsw 0
[  +0.999982] tick...
[  +0.000026] current status nvcsw 5 and nivcsw 0
[  +0.999971] tick...
[  +0.000014] current status nvcsw 6 and nivcsw 0
[  +0.397628] now we stop working!
[  +0.000048] lab1 module is being unloaded


[Oct10 16:04] lab1 module initialized, with log_sec:0 and log_nsec:100000000
[  +0.001265] now we are working!!!
[  +0.000024] current status nvcsw 0 and nivcsw 1
[  +0.099019] tick...
[  +0.000039] current status nvcsw 1 and nivcsw 1
[  +0.099965] tick...
[  +0.000535] current status nvcsw 2 and nivcsw 1
[  +0.099461] tick...
[  +0.000016] current status nvcsw 3 and nivcsw 1
[  +0.099984] tick...
[  +0.000017] current status nvcsw 4 and nivcsw 1
[  +0.099983] tick...
[  +0.000016] current status nvcsw 5 and nivcsw 1
[  +0.099986] tick...
[  +0.000016] current status nvcsw 6 and nivcsw 1
[  +0.099984] tick...
[  +0.000021] current status nvcsw 7 and nivcsw 1
[  +0.099980] tick...
[  +0.000022] current status nvcsw 8 and nivcsw 1

Comments:
1) They nearly happens synchronously, every time after the timer tick, the loop exectute once.
2) Involuntary only happens when the kernel scheduler need to reassign the cpu resource, 
but for our kernel thread, the loop just take very short time to run once, then it yields. So 
pretty much all the preemption is voluntary. 

Multi-threading Design and Evaluation
The implementation of multi-threading version just change the way we create thread and the number 
of thread we need to maintain. Replace every single thread operations into a loop, which means for 
each thread we need execute once, nomatter such operations is to create, to stop or to wake up.

System log fragment:
[Oct10 16:43] lab1 module initialized, with log_sec:1 and log_nsec:0
[  +0.018853] now we are working!!!
[  +0.000002] now we are working!!!
[  +0.000024] running on cpu2 current status nvcsw 1 and nivcsw 0
[  +0.000017] running on cpu3 current status nvcsw 1 and nivcsw 1
[  +0.000917] now we are working!!!
[  +0.000023] running on cpu1 current status nvcsw 1 and nivcsw 0
[  +0.000163] now we are working!!!
[  +0.000016] running on cpu0 current status nvcsw 1 and nivcsw 0
[  +0.998849] tick...
[  +0.000063] running on cpu0 current status nvcsw 2 and nivcsw 0
[  +0.000007] running on cpu2 current status nvcsw 2 and nivcsw 0
[  +0.000005] running on cpu3 current status nvcsw 2 and nivcsw 1
[  +0.000016] running on cpu1 current status nvcsw 2 and nivcsw 0
[  +0.999937] tick...
[  +0.000057] running on cpu0 current status nvcsw 3 and nivcsw 0
[  +0.000008] running on cpu2 current status nvcsw 3 and nivcsw 0
[  +0.000006] running on cpu3 current status nvcsw 3 and nivcsw 1
[  +0.000023] running on cpu1 current status nvcsw 3 and nivcsw 0
[  +0.999927] tick...
[  +0.000058] running on cpu0 current status nvcsw 4 and nivcsw 0
[  +0.000013] running on cpu2 current status nvcsw 4 and nivcsw 0
[  +0.000019] running on cpu1 current status nvcsw 4 and nivcsw 0
[  +0.001948] running on cpu3 current status nvcsw 4 and nivcsw 1
[  +0.997988] tick...
[  +0.000057] running on cpu0 current status nvcsw 5 and nivcsw 0
[  +0.000011] running on cpu3 current status nvcsw 5 and nivcsw 1
[  +0.000009] running on cpu2 current status nvcsw 5 and nivcsw 0
[  +0.000021] running on cpu1 current status nvcsw 5 and nivcsw 0
[  +0.999928] tick...
[  +0.000061] running on cpu0 current status nvcsw 6 and nivcsw 0
[  +0.000010] running on cpu2 current status nvcsw 6 and nivcsw 0
[  +0.000009] running on cpu3 current status nvcsw 6 and nivcsw 2
[  +0.000036] running on cpu1 current status nvcsw 6 and nivcsw 0
[  +0.999907] tick...
[  +0.000057] running on cpu0 current status nvcsw 7 and nivcsw 0
[  +0.000014] running on cpu2 current status nvcsw 7 and nivcsw 0
[  +0.000006] running on cpu3 current status nvcsw 7 and nivcsw 3
[  +0.000022] running on cpu1 current status nvcsw 7 and nivcsw 0
[Oct10 16:44] tick...
[  +0.000061] running on cpu0 current status nvcsw 8 and nivcsw 0
[  +0.000013] running on cpu3 current status nvcsw 8 and nivcsw 3
[  +0.000005] running on cpu2 current status nvcsw 8 and nivcsw 0
[  +0.000035] running on cpu1 current status nvcsw 8 and nivcsw 0
[  +0.999918] tick...
[  +0.000079] running on cpu0 current status nvcsw 9 and nivcsw 0
[  +0.000013] running on cpu1 current status nvcsw 9 and nivcsw 0
[  +0.000007] running on cpu2 current status nvcsw 9 and nivcsw 0
[  +0.000005] running on cpu3 current status nvcsw 9 and nivcsw 4
[  +0.060781] now we stop working!
[  +0.000069] now we stop working!
[  +0.000052] now we stop working!
[  +0.000041] now we stop working!
[  +0.000056] lab1 module is being unloaded


[Oct10 16:52] lab1 module initialized, with log_sec:0 and log_nsec:100000000
[  +0.013192] now we are working!!!
[  +0.000002] now we are working!!!
[  +0.000002] now we are working!!!
[  +0.000012] running on cpu2 current status nvcsw 1 and nivcsw 1
[  +0.000003] running on cpu3 current status nvcsw 1 and nivcsw 0
[  +0.000028] running on cpu1 current status nvcsw 1 and nivcsw 0
[  +0.000969] now we are working!!!
[  +0.000018] running on cpu0 current status nvcsw 1 and nivcsw 0
[  +0.098955] tick...
[  +0.000050] running on cpu0 current status nvcsw 2 and nivcsw 0
[  +0.000004] running on cpu1 current status nvcsw 2 and nivcsw 0
[  +0.000004] running on cpu2 current status nvcsw 2 and nivcsw 1
[  +0.000006] running on cpu3 current status nvcsw 2 and nivcsw 0
[  +0.099944] tick...
[  +0.000072] running on cpu1 current status nvcsw 3 and nivcsw 0
[  +0.000005] running on cpu3 current status nvcsw 3 and nivcsw 0
[  +0.000005] running on cpu2 current status nvcsw 3 and nivcsw 1
[  +0.001032] running on cpu0 current status nvcsw 3 and nivcsw 0
[  +0.098883] tick...
[  +0.000072] running on cpu1 current status nvcsw 4 and nivcsw 0
[  +0.000006] running on cpu2 current status nvcsw 4 and nivcsw 1
[  +0.000520] running on cpu0 current status nvcsw 4 and nivcsw 0
[  +0.000028] running on cpu3 current status nvcsw 4 and nivcsw 1
[  +0.099379] tick...
[  +0.000077] running on cpu2 current status nvcsw 5 and nivcsw 1
[  +0.000006] running on cpu1 current status nvcsw 5 and nivcsw 0
[  +0.000005] running on cpu3 current status nvcsw 5 and nivcsw 1
[  +0.000815] running on cpu0 current status nvcsw 5 and nivcsw 0
[  +0.099091] tick...
[  +0.000074] running on cpu1 current status nvcsw 6 and nivcsw 0
[  +0.000006] running on cpu2 current status nvcsw 6 and nivcsw 1
[  +0.000005] running on cpu3 current status nvcsw 6 and nivcsw 1
[  +0.000596] running on cpu0 current status nvcsw 6 and nivcsw 0
[  +0.099321] tick...
[  +0.000073] running on cpu2 current status nvcsw 7 and nivcsw 1
[  +0.000005] running on cpu1 current status nvcsw 7 and nivcsw 0
[  +0.000005] running on cpu3 current status nvcsw 7 and nivcsw 2
[  +0.000588] running on cpu0 current status nvcsw 7 and nivcsw 0
[  +0.099328] tick...
[  +0.000078] running on cpu2 current status nvcsw 8 and nivcsw 1
[  +0.000005] running on cpu1 current status nvcsw 8 and nivcsw 0
[  +0.000005] running on cpu3 current status nvcsw 8 and nivcsw 2
[  +0.000577] running on cpu0 current status nvcsw 8 and nivcsw 0
[  +0.099335] tick...
[  +0.000073] running on cpu2 current status nvcsw 9 and nivcsw 1
[  +0.000006] running on cpu3 current status nvcsw 9 and nivcsw 3
[  +0.000022] running on cpu1 current status nvcsw 9 and nivcsw 0
[  +0.000541] running on cpu0 current status nvcsw 9 and nivcsw 0
[  +0.099358] tick...

Comments:
1) Same as single-threaded case, each iteration happens after the timer tick. But for each thread, 
we can identify them by their pinned cpu number, and we can see that the execution sequence of 
those thread is arbitrary.
2) The ratio of voluntary preemption to involuntary preemption is nearly the same. Still the 
involuntary preemption is the rare case.


System Performance
1) The trace.dat shows that the threads usually run completely.
2) Only kernel level process can preempty them, or they already take lots of time of cpu, then 
them will be suspended by kernel scheduler.

Execution time measurements
cpu0 0.000063
cpu1 0.000054
cpu2 0.000042
cpu3 0.000043

Jitter measurements
mean 0.000041
maximum 0.000067
minimum 0.000027

Development Effort
I start the assignment from Oct 10th 10:00am to Oct 10th 17:43am. 
Nearly take me 8 hours to finish this.

