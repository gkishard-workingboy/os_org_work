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

static struct task_struct * kthread = NULL;

static int
thread_fn(void * data)
{
    // declare thread variables
    struct fs_struct * tfs;
    struct files_struct * tfiles;
    struct nsproxy * tnsproxy;
    
    struct path tpwd;
    struct dentry * tpwd_dentry;
    struct path troot;
    struct dentry * troot_dentry;

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
                printk(KERN_INFO "%s\n", troot_subdir->d_iname);
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

static int
vfs_layer_init(void)
{
    printk(KERN_INFO "Loaded vfs layer module.\n");

    kthread = kthread_create(thread_fn, NULL, "vfs_thread");
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
