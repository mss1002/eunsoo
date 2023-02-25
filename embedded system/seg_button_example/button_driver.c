#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple button driver");

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "my_button"
#define DRIVER_CLASS "MyModuleClass"

static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs){
    int to_copy, not_copied, delta;
    char tmp[2];

    /* Get amount of data to copy */
    to_copy = min(count, sizeof(tmp));

    /* Read value of button */
    if(tmp[0] & (1 << 0)){
        tmp[0] = gpio_get_value(13)+'0';
    }
    if(tmp[1] & (1 << 1)){
        tmp[1] = gpio_get_value(26)+'0';
    }
    
    /* Copy data to user */
    not_copied = copy_to_user(user_buffer, &tmp, to_copy);

    /* Calculate data */
    delta = to_copy-not_copied;

    return delta;
}

/**
 * @brief This function is called, when the device file is opened
*/
static int driver_open(struct inode *device_file, struct file *instance){
    printk("button-open was called!\n");
    return 0;
}

static int driver_close(struct inode *device_file, struct file *instance){
    printk("button-close was called!\n");
    return 0;
}

static struct file_operations fops={
    .owner=THIS_MODULE,
    .open=driver_open,
    .release=driver_close,
    .read=driver_read
};

static int __init ModuleInit(void){
    printk("Hello, kernel!\n");

    /* Allocate a device nr */
    if(alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME)<0){
        printk("Device Nr. could not be allocated!\n");
        return -1;
    }
    printk("read_write-Device Nr.Major: %d, Minor: %d was registered!\n", my_device_nr>>20, my_device_nr&&0xfffff);

    /* Create device class */
    if((my_class=class_create(THIS_MODULE, DRIVER_CLASS))==NULL){
        printk("Device class can not e created!\n");
        goto ClassError;
    }

    /* Create device file */
    if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME)==NULL){
        printk("Can not create device file!\n");
        goto FileError;
    }

    /* Initialize device file */
    cdev_init(&my_device, &fops);

    /* Registering device to kernel */
    if(cdev_add(&my_device, my_device_nr, 1)==-1){
        printk("Registering of device to kernel failed!\n");
        goto AddError;
    }

    /* GPIO 13 init */
    if(gpio_request(13, "rpi-gpio-13")){
        printk("Can not allocate GPIO 13\n");
        goto AddError;
    }

    /* Set GPIO 13 direction */
    if(gpio_direction_output(13,0)){
        printk("Can not set GPIO 13 to output!\n");
        goto Gpio13Error;
    }

    /* GPIO 26 init */
     if(gpio_request(26, "rpi-gpio-26")){
        printk("Can not allocate GPIO 26\n");
        goto AddError;
    }

    /* Set GPIO 26 direction */
    if(gpio_direction_output(26,0)){
        printk("Can not set GPIO 26 to output!\n");
        goto Gpio26Error;
    }

    return 0;
Gpio13Error:
    gpio_free(13);
Gpio26Error:
    gpio_free(26);
AddError:
    device_destroy(my_class, my_device_nr);
FileError:
    class_destroy(my_class);
ClassError:
    unregister_chrdev_region(my_device_nr, 1);
    return -1;
}

/**
 * @brief This function is called, when the module is removed from the kernel
*/
static void __exit ModuleExit(void){
    gpio_free(13);
    gpio_free(26);
    cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev_region(my_device_nr, 1);
    printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
