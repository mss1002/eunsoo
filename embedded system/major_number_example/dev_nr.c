#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>

/* Meta Information */
MODULE_LICENSE("GPL"); // 모듈의 라이센스
MODULE_AUTHOR("Johannes 4 GNU/Linux"); // 모듈의 저작자
MODULE_DESCRIPTION("Registers a device nr. and implement some callback functions"); // 모듈에 대한 설명

/**
 * @brief This function is called, when the device file is opened
*/
static int driver_open(struct inode *device_file, struct file *instance){
    printk("dev_nr - open was called!\n");
    return 0;
}

/**
 * @brief This function is called, when the device file is closed
*/
static int driver_close(struct inode *device_file, struct file *instance){
    printk("dev_nr - close was called!\n");
    return 0;
}

static struct file_operations fops={
    .owner=THIS_MODULE,
    .open=driver_open,
    .release=driver_close
}; // 파일 오퍼레이션 구조체

#define MYMAJOR 64

static int __init ModuleInit(void) // 모듈을 커널에서 적재,load
{
    int retval;
    printk("Hello, Kernel!\n");
    /* register device nr. */
    retval=register_chrdev(MYMAJOR, "my_dev_nr", &fops);
    if(retval==0){
        printk("dev_nr - registered Device number Major: %d, Minor: %d\n", MYMAJOR, 0);
    }
    else if(retval>0){
         printk("dev_nr - registered Device number Major: %d, Minor: %d\n", retval>>20, retval&0xfffff);
    }
    else{
        printk("Could not register device number!\n");
        return -1;
    }
    return 0;
}

static void __exit ModuleExit(void) // 모듈을 커널에서 제거
{
    // 문자 디바이스 드라이버 제거
    unregister_chrdev(MYMAJOR, "my_dev_nr");
    printk("Goodbye, Kernel\n");
}

module_init(ModuleInit); // 모듈 적재 시 호출되는 함수
module_exit(ModuleExit); // 모듈 제거 시 호출되는 함수
