# Lab 2

## Team Members

Zhikuan Wei <w.zhikuan@wustl.edu>  
Zhuoran Sun <zhuoran.sun@wustl.edu>  

## Module Design and Implementation

The module contains three key parts, first is the barrier part, second is the compute part, last is the module exit part.  
We design the barrier part by using two atomic variable, one denotes start of barrier（`start_guard`), and another denotes end of barrier (`end_guard`).  
At the beginning, we initialize the `start_guard` to the number of threads, and `end_guard` as zero. The barrier function first check whether the `end_guard` is zero, if not, which means this thread is entering the barrier while others is remaining in last barrier.  
We leave the thread spinning to prevent further movement of this thread. After this, we decrement `start_guard` and test whether it is zero, if so, it should increment the value of `barrier_state`, a variable measure how many times those threads pass the barrier, otherwise, this thread will spin until the last thread enter the barrier and decrement the `start_guard` to zero.  
Each thread passing the spin section will increment `end_guard`, and only one thread will end up testing the value of `end_guard` the same as number of threads. And that thread will reset both start_guard and end_guard. And then it will record the time depending on the value of `barrier_state`.  

## Module Performance 


## Name of files


## Names of other files


## Development Effort


