/* vfs_layer.c - a loadable kernel module to print out information about
 * the process.
 */
/* for kernel module */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
/* for task struct */
#include <linux/sched.h>
#include <linux/fs_struct.h>
#include <linux/fdtable.h>
#include <linux/nsproxy.h>
/* for file system */
#include <linux/path.h>
#include <linux/mount.h>
#include <linux/dcache.h>
/* for pid */
#include <linux/pid.h>
#include <linux/fdtable.h>
#include <linux/fs.h>

// pid parameter
static int nr = -1;

module_param(nr, int, 0644);

// thread task struct
static struct task_struct * kthread = NULL;

static int thread_fn(void * data)
{
    // declare thread variables
    struct fs_struct * tfs;
    struct files_struct * tfiles;
    struct nsproxy * tnsproxy;

    // declare dentry variables    
    struct path tpwd;
    struct dentry * tpwd_dentry;
    struct path troot;
    struct dentry * troot_dentry;

    // declare traversal variables
    struct dentry * troot_subdir;
    struct list_head * troot_subdir_head;
    struct list_head * troot_subdir_node;

    // log thread initialization
    printk(KERN_INFO "Hello from thread %s.\n", current->comm);

    // thread initialization
    // task struct
    tfs = current->fs;
    printk(KERN_INFO "Filesystem information struct at %p.\n", tfs);
    tfiles = current->files;
    printk(KERN_INFO "Open file information struct at %p.\n", tfiles);
    tnsproxy = current->nsproxy;
    printk(KERN_INFO "Namespace proxy struct at %p.\n", tnsproxy);
    
    // file system
    get_fs_root(tfs, &troot);
    troot_dentry = troot.dentry;
    printk(KERN_INFO "Root dentry struct at %p.\n", troot_dentry);
    get_fs_pwd(tfs, &tpwd);
    tpwd_dentry = tpwd.dentry;
    printk(KERN_INFO "PWD dentry struct at %p.\n", tpwd_dentry);

    if (troot_dentry == tpwd_dentry) {
        if (troot_dentry != NULL) {
            printk(KERN_INFO "Name of root (working) directory is %s.\n", troot_dentry->d_iname);
        }
        else {
            printk(KERN_WARNING "Name of root (working) directory is NULL.\n");
        }
    }   
    else {
        if (troot_dentry != NULL) {
            printk(KERN_INFO "Name of root directory is %s.\n", troot_dentry->d_iname);
        }
        else {
            printk(KERN_WARNING "Name of root directory is NULL.\n");
        }
        if (tpwd_dentry != NULL) {
            printk(KERN_INFO "Name of working directory is %s.\n", tpwd_dentry->d_iname);
        }
        else {
            printk(KERN_WARNING "Name of working directory is NULL.\n");
        }
    }

    // traverse subdir of root
    if (troot_dentry != NULL) {
        // start at head
        troot_subdir_head = &troot_dentry->d_subdirs;
        troot_subdir_node = troot_subdir_head;
        // traverse until back to head
        do {
            // get subdir
            troot_subdir = container_of(troot_subdir_node, struct dentry, d_child);
            // print file name
            if (troot_subdir != NULL) {
                // only if the subdirs is not empty
                if (!((troot_subdir->d_subdirs).next != &(troot_subdir->d_subdirs))) {
                    printk(KERN_INFO "%s\n", troot_subdir->d_iname);
                }
            }
            else {
                printk(KERN_WARNING "NULL\n");
            }
            // move on to the next node
            troot_subdir_node = troot_subdir_node->next;
        } while (troot_subdir_node != troot_subdir_head);
    }

    // wait for stop
    while (!kthread_should_stop()) {
        schedule();
    }
    
    // log thread destruction
    printk(KERN_INFO "Goodbye from thread %s.\n", current->comm);
    
    // thread destruction
 
    return 0;
}

static int thread_pid(void * data)
{
    // declare pid variables
    struct pid * p_ptr;
    struct task_struct * ts_ptr;
    struct files_struct * fs_ptr;
    struct file * f_ptr;
    const struct file_operations * fo_ptr;
    loff_t offset = 0;
    const char * message = "Hi from kernel thread.\n";
    size_t message_len = strlen(message) + 1;

    // log thread initialization
    printk(KERN_INFO "Hello from thread %s.\n", current->comm);

    p_ptr = find_get_pid(nr);

    if (p_ptr == NULL) {
        // log null pointer
        printk(KERN_WARNING "Failed to find pid %d.\n", nr);
        return -1;
    }
    
    ts_ptr = pid_task(p_ptr, PIDTYPE_PID);

    if (ts_ptr == NULL) {
        // log null pointer
        printk(KERN_WARNING "Failed to set task to pid %d.\n", nr);
        return -2;
    }

    fs_ptr = ts_ptr->files;

    // standard output have a file descriptor of 1
    f_ptr = fcheck_files(fs_ptr, 1);

    fo_ptr = f_ptr->f_op;

    fo_ptr->write(f_ptr, message, message_len, &offset);

    // wait for stop
    while (!kthread_should_stop()) {
        schedule();
    }
    
    // log thread destruction
    printk(KERN_INFO "Goodbye from thread %s.\n", current->comm);
    
    // thread destruction
 
    return 0;
}

static int
vfs_layer_init(void)
{
    printk(KERN_INFO "Loaded vfs layer module.\n");
    if (nr == -1) {
        // create ls thread
        kthread = kthread_create(thread_fn, NULL, "vfs_thread");
    }
    else {
        // create pid thread instead
        kthread = kthread_create(thread_pid, NULL, "vfs_pid_thread");
    }
    if (IS_ERR(kthread)) {
        printk(KERN_ERR "Failed to create kernel thread\n");
        return PTR_ERR(kthread);
    }
    
    wake_up_process(kthread);

    return 0;
}

static void 
vfs_layer_exit(void)
{
    kthread_stop(kthread);
    printk(KERN_INFO "Unloaded vfs layer module.\n");
}

module_init(vfs_layer_init);
module_exit(vfs_layer_exit);

MODULE_LICENSE ("GPL");
