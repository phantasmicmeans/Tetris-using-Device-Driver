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

#define CSEMAD_DOT_MATRIX_MAJOR 262
#define CSEMAD_DOT_MATRIX_NAME "csemad_dot_matrix"
#define CSEMAD_DOT_MATRIX_ADDRESS 0x08000210

// global variable definitions
static int ledport_usage = 0;
static unsigned char *csemad_dot_matrix_addr;

// functions dregister_chrdeveclarations
//ssize_t csemad_dot_matrix_read(struct file *inode, char *gdata, size_t length, loff_t *                        off_what);
ssize_t csemad_dot_matrix_write(struct file *inode, const  char *gdata, size_t length, loff_t *off_what);
int csemad_dot_matrix_open(struct inode *minode, struct file *mfile);
int csemad_dot_matrix_release(struct inode *minode, struct file *mfile);

// device file operation
struct file_operations csemad_dot_matrix_fops = {
   .owner = THIS_MODULE,
   .open = csemad_dot_matrix_open,
   .write = csemad_dot_matrix_write,
   //   .read = csemad_dot_matrix_read,
   .release = csemad_dot_matrix_release,
};

// initialize LED port
int __init csemad_dot_matrix_init(void) {

   int result;

   result = register_chrdev(CSEMAD_DOT_MATRIX_MAJOR, CSEMAD_DOT_MATRIX_NAME, &csemad_dot_matrix_fops);

   if (result < 0) {
      printk(KERN_WARNING "failed to register yout device");
      return result;
   }
   csemad_dot_matrix_addr = ioremap(CSEMAD_DOT_MATRIX_ADDRESS, 0x1000);
   return 0;
}

// release and exit LED port
void __exit csemad_dot_matrix_exit(void) {

   iounmap(csemad_dot_matrix_addr);
   unregister_chrdev(CSEMAD_DOT_MATRIX_MAJOR, CSEMAD_DOT_MATRIX_NAME);
}

// open LED port
int csemad_dot_matrix_open(struct inode *minode, struct file *mfile) {

   if (ledport_usage != 0) {
      return -EBUSY;
   }
   ledport_usage = 1;
   return 0;
}

// close LED port
int csemad_dot_matrix_release(struct inode *minode, struct file *mfile) {

   ledport_usage = 0;

   return 0;
}

// read from LED port
/*
ssize_t csemad_dot_matrix_read(struct file *inode, char *gdata, size_t length, loff_t *                        off_what) {

unsigned long  result = copy_to_user(gdata, csemad_dot_matrix_addr, len                        gth);
if (result != 0){
return -EFAULT;
}
return length;
}*/

// write to LED port
ssize_t csemad_dot_matrix_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what) {

   int i;
   unsigned long result;
   for (i = 0;i<10;i++)
   {
      csemad_dot_matrix_addr = ioremap(CSEMAD_DOT_MATRIX_ADDRESS + (i * 2), 0x1000);

      result = copy_from_user(csemad_dot_matrix_addr, &gdata[i], length);



      if (result != 0) {

         return -EFAULT;
      }
   }
   return length;
}

module_init(csemad_dot_matrix_init);
module_exit(csemad_dot_matrix_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CSEMAD");


