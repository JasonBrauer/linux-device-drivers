/*
 * main.c -- the bare scull char module
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 */

#include <linux/cdev.h> /* cdev struct and associated helper functions */
#include <linux/errno.h> /* error codes */
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/fs.h> /* file_operations, file, inode structs */
#include <linux/init.h> /* initialization and cleanup functions */
#include <linux/kernel.h>	/* printk(), container_of() */
#include <linux/kdev_t.h> /* dev_t macros*/
#include <linux/module.h> /* symbols and functions needed by loadable modules */
#include <linux/moduleparam.h> /* module_param macro */
#include <linux/slab.h> /* kernel memory management - kfree, kmalloc*/
#include <linux/types.h> /* dev_t */


#include "scull.h" /* device number defs, public variables and functions */


/*
 * Our parameters which can be set at load time.
 */

int scull_major =   SCULL_MAJOR;
int scull_minor =   0;
int scull_nr_devs = SCULL_NR_DEVS;	/* number of bare scull devices */
int scull_quantum = SCULL_QUANTUM;
int scull_qset =    SCULL_QSET;

module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_quantum, int, S_IRUGO);
module_param(scull_qset, int, S_IRUGO);

MODULE_AUTHOR("Alessandro Rubini, Jonathan Corbet");
MODULE_LICENSE("Dual BSD/GPL");


int scull_init_module(void)
{
	int result, i;
	dev_t dev = 0;

	/*
	 * Get a range of minor numbers to work with, asking for a dynamic
	 * major unless directed otherwise at load time.
	 */
	if (scull_major) {
        /* register with explicitly defined major and minor device numbers */
		dev = MKDEV(scull_major, scull_minor);
		result = register_chrdev_region(dev, scull_nr_devs, "scull");
	} else {
        /* dynamically allocate major device number */
		result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs,
				"scull");
		scull_major = MAJOR(dev);
	}
	if (result < 0) {
		printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
		return result;
	}
}


/* defines driver operations */
struct file_operations scull_fops = {
	.owner =    THIS_MODULE,
	.llseek =   scull_llseek,
	.read =     scull_read,
	.write =    scull_write,
	.unlocked_ioctl = scull_ioctl,
	.open =     scull_open,
	.release =  scull_release,
};


/*
 * Set up the char_dev structure for this device. The cdev structure interfaces our device to the 
 * kernel
 */
static void scull_setup_cdev(struct scull_dev *dev, int index)
{
	/* make a dev_t with the existing allocated major device number and incremented minor number */
	int err, devno = MKDEV(scull_major, scull_minor + index);
    
	/* inititalize an existing cdev struct */
	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	/* tell the kernel about the new char device -> device is "live" now */
	err = cdev_add(&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}


/* Any initialization in preparation for later operations: 
 * check for device specific errors, 
 * initialize the device if it is being opened for the first time, 
 * update the file operations (f_op) pointer if necessary, 
 * allocate and fill any data structure to be put in the file pointer private data field (filp->private_data), 
 */
int scull_open(struct inode *inode, struct file *filp)
{
	struct scull_dev *dev; /* device information */

	/* returns pointer to the scull_dev structure that holds the cdev pointer returned by the 
	 * provided inode 
	 */
	dev = container_of(inode->i_cdev, struct scull_dev, cdev);
	filp->private_data = dev; /* for other methods */

	/* now trim to 0 the length of the device if open was write-only */
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
		if (mutex_lock_interruptible(&dev->lock))
			return -ERESTARTSYS;
		scull_trim(dev); /* ignore errors */
		mutex_unlock(&dev->lock);
	}
	return 0;          /* success */
}


/*
 * Empty out the scull device; must be called with the device
 * semaphore held.
 */
int scull_trim(struct scull_dev *dev)
{
	struct scull_qset *next, *dptr;
	int qset = dev->qset;   /* "dev" is not-null */
	int i;

	for (dptr = dev->data; dptr; dptr = next) { /* all the list items */
		if (dptr->data) {
			for (i = 0; i < qset; i++)
				kfree(dptr->data[i]);
			kfree(dptr->data);
			dptr->data = NULL;
		}
		next = dptr->next;
		kfree(dptr);
	}
	dev->size = 0;
	dev->quantum = scull_quantum;
	dev->qset = scull_qset;
	dev->data = NULL;
	return 0;
}