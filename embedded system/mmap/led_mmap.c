#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define BCM2711_PERI_BASE 0xFE000000
#define BLOCK_SIZE (4*1024)
#define PAGE_SIZE (4*1024)
#define GPIO_BASE (BCM2711_PERI_BASE+0x200000)

#define GPFSEL0 0x00
#define GPSET0 0x1c
#define GPCLR0 0x28

int main(int argc, char **argv)
{
    int data;
    int memfd;
    char *addr_gpio;
    volatile unsigned int *gpio;

    if(argc<2){
        printf("put data 0/1\n");
        return -1;
    }

    memfd=open("/dev/mem", O_RDWR|O_SYNC);
    if(memfd<0){
        printf("mem open fail\n");
        return -1;
    }

    addr_gpio=(char*)mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, memfd, GPIO_BASE);

    if (addr_gpio==MAP_FAILED)
    {
        printf("mmap fail\n");
        return -1;
    }
    
    gpio=(volatile unsigned int*)addr_gpio;

    gpio[GPFSEL0/4]|=(1<<12);

    if(argv[1][0]=='1'){
        gpio[GPSET0/4]|=(1<<4);
        printf("set led on\n");
    }
    else if(argv[1][0]=='0'){
        gpio[GPCLR0/4]|=(1<<4);
        printf("set led off\n");
    }

    munmap(addr_gpio,BLOCK_SIZE);
    close(memfd);

    return 0;
}
