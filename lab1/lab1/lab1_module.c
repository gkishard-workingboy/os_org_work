#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>
#include <linux/kthread.h>

#define D_SEC 1
#define D_NSEC 0

static unsigned long log_sec = D_SEC;
static unsigned long log_nsec = D_NSEC;
module_param(log_sec, ulong, 0);
module_param(log_nsec, ulong, 0);

static ktime_t interv;
static struct hrtimer timer;
static struct task_struct *task = NULL;

static enum hrtimer_restart tick(struct hrtimer *htimer) {
   	printk(KERN_ALERT "tick...\n");
	wake_up_process(task);
	hrtimer_forward_now(htimer, interv);
	return HRTIMER_RESTART;
}

static int workingfn(void *data) {
	bool stop_sign;
	printk(KERN_ALERT "now we are working!!!\n");
	while (true) {
		printk(KERN_ALERT "current status nvcsw %lu and nivcsw %lu\n", current->nvcsw, current->nivcsw);
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

    printk(KERN_ALERT "lab1 module initialized, with log_sec:%lu and log_nsec:%lu\n", log_sec, log_nsec);
    interv = ktime_set(log_sec, log_nsec);
        
    task = kthread_run(&workingfn, NULL, "lab1 single thread");
    if (task == NULL) {
     	printk(KERN_ALERT "kthread_run is failed\n");
		return -1;
    }
	
    hrtimer_init(&timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    timer.function = &tick;
    hrtimer_start(&timer, interv, HRTIMER_MODE_REL);
    
	return 0;
}

static void lab1_exit(void)
{
	kthread_stop(task);
    hrtimer_cancel(&timer);
    printk(KERN_ALERT "lab1 module is being unloaded\n");
}

module_init(lab1_init);
module_exit(lab1_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR ("Zhikuan Wei");
MODULE_DESCRIPTION("for lab1");
