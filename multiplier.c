#include <linux/module.h>   // Needed by all modules
#include <linux/moduleparam.h>  /* Needed for module parameters */
#include <linux/kernel.h>   // Needed for KERN_* and printk
#include <linux/init.h>     // Needed for __init and __exit macros
#include <linux/fs.h>	   /* Provides file ops structure */
#include <asm/io.h>         // Needed for IO reads and writes
#include "xparameters.h"    // Needed for IO reads and writes
#include <linux/ioport.h>   // Used for io memory allocation
#include <linux/sched.h>   /* Provides access to the "current" process
			      task structure */
#include <asm/uaccess.h>   /* Provides utilities to bring user space
			      data into kernel space.  Note, it is
			      processor arch specific. */
#include<linux/slab.h>

// From xparameters.h, physical address of multiplier
#define PHY_ADDR XPAR_MULTIPLY_0_S00_AXI_BASEADDR 

// Size of physical address range for multiply
#define MEMSIZE XPAR_MULTIPLY_0_S00_AXI_HIGHADDR \
        - XPAR_MULTIPLY_0_S00_AXI_BASEADDR + 1

// Name of the device
#define DEVICE_NAME "multiplier"

#define BUF_LEN 8

// Major number assigned to the device driver 
static int Major;

// virtual address pointing to multiplier
void* virt_addr; 

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

/* This structure defines the function pointers to our functions for
   opening, closing, reading and writing the device file.  There are
   lots of other pointers in this structure which we are not using,
   see the whole definition in linux/fs.h */
static struct file_operations fops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};

/* This function is run upon module load. This is where you setup data
   structures and reserve resources used by the module */
static int __init my_init(void)
{
    // Linux kernel's version of printf
    printk(KERN_INFO "Mapping virtual address...\n");
    
    // map virtual address to multiplier physical address
    virt_addr = ioremap(PHY_ADDR, MEMSIZE);

    // use ioremap, print the physical and virtual address 
    printk("Physical address of the multiplication peripheral: %x\n", PHY_ADDR);
    printk("Virtual address of the multiplication peripheral: %x\n", 
	*(int *)virt_addr);

    // register the device
    Major = register_chrdev(0, DEVICE_NAME, &fops);

    /* Negative values indicate a problem */
    if (Major < 0) {		
    /* Make sure you release any other resources you've already
       grabbed if you get here so you don't leave the kernel in a
       broken state. */
      printk(KERN_ALERT "Registering char device failed with %d\n", Major);
      return Major;
    }

    // print the major number assigned to the device
    printk("Char device registered with major number %d\n", Major);

    printk(KERN_INFO "Run:\nmknod /dev/%s c %d 0' to register device.\n", 
	DEVICE_NAME, Major);
    
    // A non 0 return means init_module failed; module can't be loaded
    return 0;
}

/* This function is run just prior to the module's removal from the system.
   You should release ALL resources used by your module here (otherwise be
   prepared for a reboot). */
static void __exit my_exit(void)
{
    printk(KERN_ALERT "unmapping virtual address space...\n");
    unregister_chrdev(Major, DEVICE_NAME);
    iounmap((void*)virt_addr);
}

static int device_open(struct inode *inode, struct file *file)
{
    printk("Device is opened!\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    printk("Device is closed!\n");
    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, 
loff_t *offset)
{
    int bytes_read = 0;
   
    for (bytes_read = 0; bytes_read < length; bytes_read++){
      put_user((char)ioread8(virt_addr+bytes_read), buffer + bytes_read);
    }

    return bytes_read;
}

static ssize_t device_write(struct file *flip, const char __user *buffer, 
size_t length, loff_t *offset)
{
    int bytes_written = 0;
    char buf;
 
    for (bytes_written = 0; bytes_written < length; bytes_written ++){
      get_user(buf, buffer + bytes_written);
      iowrite8(buf, virt_addr + bytes_written);
    }
   
    return bytes_written;
}

// These define info that can be displayed by modinfo
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ECEN449 Student (and others)");
MODULE_DESCRIPTION("Simple multiplier module");

// Here we define which functions we want to use for initialization and cleanup
module_init(my_init);
module_exit(my_exit);
