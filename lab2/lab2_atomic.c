#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/spinlock_types.h>
#include <linux/ktime.h>
#include <linux/slab.h>

#define LINE_WIDTH 8
#define FIRST_PRIME 2
#define RUNNING 1

static unsigned long num_threads = 1;
static unsigned long upper_bound = 10;

module_param (num_threads, ulong, 0644);
module_param (upper_bound, ulong, 0644);

static int *counters = 0;
static atomic_t *numbers = 0;
static volatile int global_index = FIRST_PRIME;
static volatile int barrier_state = 0;
static volatile int begin_flag = 0;
static atomic_t start_guard, end_guard;
static ktime_t init_begin = 0ull, threads_begin = 0ull, threads_end = 0ull;
DEFINE_SPINLOCK(index_lock);


static int sbarrier(void) {
    while (atomic_read(&end_guard) != 0) {}
    pr_info("enter barrier");
    if (atomic_dec_and_test(&start_guard)) {
        barrier_state++;
        if (barrier_state == 1) {
            pr_info("threads_begin!\n");
            threads_begin = ktime_get();
        } else if (barrier_state == 2) {
            pr_info("threads_end!\n");
            threads_end = ktime_get();
        }
        atomic_set(&start_guard, num_threads);
    } else {
        while (atomic_read(&start_guard) != num_threads) {}
    }

    //
    if (atomic_add_return(1, &end_guard) == num_threads) {
        atomic_set(&end_guard, 0);
        pr_info("last threads\n");
    } else{
        pr_info("else threads\n");
    }
    return 0;
}

static void compute(int * counter) {
    int local_index , original;
    do {
        // Critical region, accessing global index.
        spin_lock(&index_lock);
        // Find next prime by finding the next number that is not
        // crossed out.
        for (; global_index <= upper_bound; ++global_index) {
            if (atomic_read(numbers+global_index) != 0) break;
        }
        // Store the next prime in a local variable and increment the
        // global counter.
        original = global_index++;
        spin_unlock(&index_lock);
        
        // Check if the prime has surpassed the upper bound.
        if (original <= upper_bound) {
            // Local work to cross out all multiples of original.
            for (local_index = MIN_PRIME*original ; local_index <= upper_bound; local_index += original) {
                // Cross out number.
                numbers[local_index] = 0;
                // Increment counter.
                (*counter)++;
            }
        }
    } while (global_index <= upper_bound);
    
    // Log finish computing
    pr_info("Finish computing.\n");
}

static int sieve_fn(void* data) {
    int *counter = (int *)data;
    sbarrier();
    compute(counter);
    sbarrier();
    if (begin_flag == 1) begin_flag = 0;
    return 0;
}


static int lab2_init(void) {
    int i;
    if (num_threads < 1 || upper_bound < FIRST_PRIME) {
        printk(KERN_ERR "invalid parameter num_threads {%lu} should be at least 1 and upper_bound {%lu} should be at least 2.\n", num_threads, upper_bound);
        return -1;
    }
    init_begin = ktime_get();

    numbers = (atomic_t*)kmalloc(sizeof(atomic_t) * (upper_bound+1), GFP_KERNEL);
    if (numbers == NULL) {
        printk(KERN_ERR "kmalloc numbers fail.\n");
        return -1;
    }

    counters = (int*)kmalloc(sizeof(int) * num_threads, GFP_KERNEL);
    if (counters == NULL) {
        printk(KERN_ERR "kmalloc counters fail.\n");
    }

    for (i = 0; i < num_threads; ++i) {
        counters[i] = 0;
    }

    for (i = 0; i <= upper_bound; ++i) {
        atomic_set(numbers+i, i);
    }

    atomic_set(&end_guard, 0);
    atomic_set(&start_guard, num_threads);

    begin_flag = RUNNING;
    for (i = 0; i < num_threads; ++i) {
        // Spawn worker threads, assign counters.
        kthread_run(&sieve_fn, (void *)(counters+i), "lab2_thread{%2d}", i);
    }

    return 0;
}

static void lab2_exit(void) {
    int i, j, count_sum, prime_nums, non_prime_nums;
    struct timespec retval;
    if (begin_flag == RUNNING) {
        pr_err("threads are still working.");
        return;
    }
    if (numbers != NULL) {
        if (barrier_state > 0) {
            printk(KERN_INFO "prime numbers:");
            j = LINE_WIDTH-1;
            prime_nums = 0;
            for (i = FIRST_PRIME; i <= upper_bound; ++i) {
                if (atomic_read(numbers+i) != 0) {
                    prime_nums++;
                    if (++j == LINE_WIDTH) {
                        printk (KERN_INFO "%5d", atomic_read(numbers+i));
                        j = 0;
                    } else {
                        printk (KERN_CONT " %5d", atomic_read(numbers+i));
                    }
                }
            }
        }
        kfree(numbers);
        numbers = NULL;
    }

    if (counters != NULL) {
        if (barrier_state > 0) {
            printk(KERN_INFO "counters:");
            j = LINE_WIDTH-1;
            count_sum = 0;
            for (i = 0; i < num_threads; ++i) {
                count_sum += counters[i];
                if (++j == LINE_WIDTH) {
                    printk (KERN_INFO "%5d", counters[i]);
                    j = 0;
                } else {
                    printk (KERN_CONT " %5d", counters[i]);
                }
            } 
            non_prime_nums = upper_bound - 1 - prime_nums;
            printk (KERN_INFO "total number of primes: %d, non-primes: %d, unnecessary cross out: %d", prime_nums, non_prime_nums, count_sum - non_prime_nums);
            printk (KERN_INFO "upper bounds: %lu, number of threads: %lu", upper_bound, num_threads);
            retval = ktime_to_timespec(ktime_sub(threads_begin, init_begin));
            printk (KERN_INFO "time spent for setting up module: %ld.%.9ld\n", retval.tv_sec, retval.tv_nsec);
            retval = ktime_to_timespec(ktime_sub(threads_end, threads_begin));
            printk (KERN_INFO "time spent for processing primes: %ld.%.9ld\n", retval.tv_sec, retval.tv_nsec);
        }
        kfree(counters);
        counters = NULL;
    }
    return;
}

module_init(lab2_init);
module_exit(lab2_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhikuan Wei");
MODULE_DESCRIPTION("atomic version");
