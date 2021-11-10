Lab 2


Zhikuan Wei
w.zhikuan@wustl.edu
Zhuoran Sun
zhuoran.sun@wustl.edu


Module Design and Implementation
The module contains three key parts, first is the barrier part, second is the compute part, last is the module exit part.

We design the barrier part by using two atomic variable, one denotes start of barrier（start_guard), and another denotes end 
of barrier(end_guard). At the beginning, we initialize the start_guard as the number of threads, and end_guard as zero. So the 
barrier function first check whether the end_guard is zero, if not, which means this thread is entering the barrier while 
others is remaining in last barrier. So we should spin it to prevent further movement of this thread. After this, we decrement 
start_guard and test whether it is zero, if so, it should increment the value of barrier_state, a variable measure how many 
time those threads pass the barrier, and record the timestamp based on the barrier_state. Otherwise, this thread will spin 
itself until the last thread enter the barrier and modify the start_guard to num_threads. Each thread passing the spin section 
will increment end_guard, and only one thread will end up testing the value of end_guard the same as number of threads. And 
that thread will reset end_guard to zero, otherwise, no thread reenter this barrier function will pass due to the primary 
testing of the value of end_guard.

We design the compute function by using a spin lock. At the beginning, each thread firstly tries to acquire the lock and then 
just search from the global_index unitll find a non-zero number in the array, which we treat that number as "prime", implementing the 
Eratosthenes seive. And each time the thread cross out a number, we increament the corresponding counter of that thread. Whenever 
the global_index is not less than upper_bound, then the funciton will return without doing anything. And in the second module, we 
just replace all the basic integer operations to atomic operations by change the data type of our array to atomic_t.

We design the exit part as an very safe exit function. Firstly, we use a variable named begin_flag denoting the whole program is in progress.
If it is still in progress, we just return the exit funciton as what the lab2 requirement told us to do.
After this, we judge whether the pointer to the array "numbers" is NULL or not, if yes, we just do nothing, otherwise we will maka a further 
judgement whether the barrier_state is greater than zero, is so, which means the whole algorithm is already done, then we just traverse 
the array and get the statistical data we want. Otherwise, which means the module has something crashs in init process, we just need to free 
the memory location of numbers.
Then we probably do the analogous operations to counters. And make sure all the memory location will be free and pointer will be set to NULL 
at the end of the function.


Module Performance 
We have respectively test the performance of two module in 1, 2 and 4 threads with upper_bound as 100, 200, 500, 1000, 2000, 4000, 8000,
10000, 20000 and 100000. We save them as two graph named "lab2.png" and "lab2atomic.png". 
And we notice that first the timing of initialization is changing relatively to the change of number of threads, and the timing of computation 
section is changing relatively to the change of upper_bound. And second, we find out the unnecessary cross will becomes larger under the same 
upper bound with larger number of threads.


Name of files
lab2.png
lab2atomic.png
mocule_performance.xlsx


Names of other files
lab2.c
lab2_atomic.c

Development Effort
Zhikuan Wei spend nearly 14 hours on this project. 

