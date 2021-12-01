CSE 442S 12/1/2021 Studio 18

## Q1:
Zhuoran Sun
Zhikuan Wei

## Q2:
The program is terminated the moment I pressed CTRL+C and sends SIGINT.
```
i: 3809
i: 3810
i: 3811
i: 3812
i: 3813
i: 3814
i: 3815
^C
```

## Q3:
The purpose of the called array is to store the i when SIGINT is sent.

## Q4:
The program does not terminate when I press CTRL+C and sends SIGINT because the handler 
is called instead of executing the default behavior.
```
i: 499996
i: 499997
i: 499998
i: 499999
17440 was possibly interrupted
74458 was possibly interrupted
127766 was possibly interrupted
197712 was possibly interrupted
216158 was possibly interrupted
230326 was possibly interrupted
250014 was possibly interrupted
265029 was possibly interrupted
279238 was possibly interrupted
296014 was possibly interrupted
325735 was possibly interrupted
343072 was possibly interrupted
355899 was possibly interrupted
370074 was possibly interrupted
385066 was possibly interrupted
400296 was possibly interrupted
414694 was possibly interrupted
445146 was possibly interrupted
458900 was possibly interrupted
476426 was possibly interrupted
491146 was possibly interrupted
```

## Q5:
The program gracefully terminated by breaking the loop and then printing where it was
possibly interrupted when I press CTRL+C and sends SIGINT.
```
i: 22259
i: 22260
i: 22261
i: ^C
23796 was possibly interrupted
```

## Q6:
189286 was the last number printed, this makes sense because 189287 is when SIGINT is 
sent to the program according to the handler. However, the number went back to 188879 
after SIGINT is called. This is probably because the printf buffer is not fully printed
when the signal was sent. As seen in `man 7 signal-safety`, printf() is not async-signal-
safe, thus causing this problem.
```
i: 189285
i: 189286
Caught SIGINT.

i: 188879
i: 188880
--------------
i: 499998
i: 499999
189287 was possibly interrupted
250317 was possibly interrupted
321178 was possibly interrupted
387943 was possibly interrupted
412109 was possibly interrupted
440211 was possibly interrupted
```

## Q7:
The output is in order, and Caught SIGINT is printed in midst of another write.
```
i: 69954
i: 69955
i:Caught SIGINT.
 69956
i: 69957
i: 69958
```

## Q8:
The program first register the signal handler for SIGUSR1 and SIGUSR2 and prints out its
pid. Then the program  spins until done is set to 1, which only happens when SIGUSER2 is 
sent to the program. Then the program exits and print how many SIGUSR1 the program has 
received during its lifetime.

## Q9:
The receiver only received SIGUSR1 24 times rather than the 100 times that the sender 
sent out. This makes sense because signals are not queued, so if SIGUSR1 is sent again 
before the receiver has handled the signal, it will miss.
Receiver Side:
```
 ./receiver 
pid: 2568
SIGUSR1 received 24 times
```
Sender Side:
```
pi@jordansunpi:~/Documents/CSE422S/lab2/operating_system_organization/Studio18 $ ./sender 2568 100
target killed.
```
