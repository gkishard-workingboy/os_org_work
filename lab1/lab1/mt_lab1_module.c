#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>
#include <linux/kthread.h>

#define CPUS 4
#define D_SEC 1
#define D_NSEC 0

static unsigned long log_sec = D_SEC;
static unsigned long log_nsec = D_NSEC;
module_param(log_sec, ulong, 0);
module_param(log_nsec, ulong, 0);

static ktime_t interv;
static struct hrtimer timer;
static struct task_struct *tasks[CPUS] = {0};

static enum hrtimer_restart tick(struct hrtimer *htimer) {
	int i;
   	printk(KERN_ALERT "tick...\n");
	for (i = 0; i < CPUS; ++i) {
		wake_up_process(tasks[i]);
	}   
	hrtimer_forward_now(htimer, interv);
	return HRTIMER_RESTART;
}

static int workingfn(void *data) {
	bool stop_sign;
	printk(KERN_ALERT "now we are working!!!\n");
	while (true) {
		printk(KERN_ALERT "running on cpu%d current status nvcsw %lu and nivcsw %lu\n",\
		smp_processor_id(), current->nvcsw, current->nivcsw);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
		if ((stop_sign = kthread_should_stop())) {
			break;
		}
	}
	printk(KERN_ALERT "now we stop working!\n");
	return 0;
}

static int lab1_init(void)
{
	int i;
    printk(KERN_ALERT "lab1 module initialized, with log_sec:%lu and log_nsec:%lu\n", log_sec, log_nsec);
    interv = ktime_set(log_sec, log_nsec);
    
	for (i = 0; i < CPUS; ++i) {
		tasks[i] = kthread_create(&workingfn, NULL, "mt_thread{%d}", i);
    	if (tasks[i] == NULL) {
     		printk(KERN_ALERT "%dth kthread_run is failed\n", i);
			return -1;
    	}
		kthread_bind(tasks[i], i);
	}

	for (i = 0; i < CPUS; ++i) {
		wake_up_process(tasks[i]);
	}   
	
    hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    timer.function = &tick;
    hrtimer_start(&timer, interv, HRTIMER_MODE_REL);
    
	return 0;
}

static void lab1_exit(void)
{
	int i;
	for (i = 0; i < CPUS; ++i) {
		kthread_stop(tasks[i]);
	}
    hrtimer_cancel(&timer);
    printk(KERN_ALERT "lab1 module is being unloaded\n");
}

module_init(lab1_init);
module_exit(lab1_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR ("Zhikuan Wei");
MODULE_DESCRIPTION("for lab1");
