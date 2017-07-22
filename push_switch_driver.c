#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/version.h>

#define CSEMAD_PUSH_SWITCH_MAJOR 265
#define CSEMAD_PUSH_SWITCH_NAME "csemad_push_switch_driver"
#define CSEMAD_PUSH_SWITCH_ADDRESS 0x08000050

// global variable definitions
static int ledport_usage = 0;
static unsigned char *csemad_push_switch_addr;

// functions dregister_chrdeveclarations
ssize_t csemad_push_switch_read(struct file *inode, char *gdata, size_t length, loff_t *off_what);
//ssize_t csemad_push_switch_write(struct file *inode,const  char *gdata, size_t length, loff_t *off_what);
int csemad_push_switch_open(struct inode *minode, struct file *mfile);
int csemad_push_switch_release(struct inode *minode, struct file *mfile);

// device file operation
struct file_operations csemad_push_switch_fops = {
   .owner = THIS_MODULE,
   .open = csemad_push_switch_open,
   //.write = csemad_push_switch_write,
   .read = csemad_push_switch_read,
   .release = csemad_push_switch_release,
};

// initialize LED port
int __init csemad_push_switch_init(void) {

   int result;

   result = register_chrdev(CSEMAD_PUSH_SWITCH_MAJOR, CSEMAD_PUSH_SWITCH_NAME, &csemad_push_switch_fops);

   if (result < 0) {
      printk(KERN_WARNING "failed to register yout device");
      return result;
   }
   csemad_push_switch_addr = ioremap(CSEMAD_PUSH_SWITCH_ADDRESS, 0x1000);
   return 0;
}

// release and exit LED port
void __exit csemad_push_switch_exit(void) {

   iounmap(csemad_push_switch_addr);
   unregister_chrdev(CSEMAD_PUSH_SWITCH_MAJOR, CSEMAD_PUSH_SWITCH_NAME);
}

// open LED port
int csemad_push_switch_open(struct inode *minode, struct file *mfile) {

   if (ledport_usage != 0) {
      return -EBUSY;
   }
   ledport_usage = 1;
   return 0;
}

// close LED port
int csemad_push_switch_release(struct inode *minode, struct file *mfile) {

   ledport_usage = 0;

   return 0;
}

// read from LED port
ssize_t csemad_push_switch_read(struct file *inode, char *gdata, size_t length, loff_t *off_what) {


   int i;
   unsigned long result;
   for (i = 0; i<9; i++)
   {
      csemad_push_switch_addr = ioremap(CSEMAD_PUSH_SWITCH_ADDRESS + (i * 2), 0x1000);

      result = copy_to_user(&gdata[i], csemad_push_switch_addr, length);

      if (result != 0) {
         return -EFAULT;
      }
   }

   return length;
}

/*
// write to LED port
ssize_t csemad_push_switch_write(struct file *inode,const char *gdata, size_t length, loff_t *off_what) {

unsigned long result = copy_from_user(csemad_push_switch_addr,gdata, length);

if (result != 0){

return -EFAULT;
}

return length;
}

*/
module_init(csemad_push_switch_init);
module_exit(csemad_push_switch_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CSEMAD");



