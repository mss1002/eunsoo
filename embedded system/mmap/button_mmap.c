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
#define GPFSEL1 0x04
#define GPLEV0 0x34

int main(int argc, char **argv)
{
    int data;
    int memfd;
    char *addr_gpio;
    volatile unsigned int *gpio;
    int tmp;
    int prev;

    memfd=open("/dev/mem", O_RDWR|O_SYNC);
    if(memfd<0){
        printf("mem open fail\n");
        return -1;
    }

    addr_gpio=(char*)mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, memfd, GPIO_BASE);

    if(addr_gpio==MAP_FAILED){
        printf("mmap fail\n");
        return -1;
    }

    gpio=(volatile unsigned int*)addr_gpio;

    while(1){
        prev=tmp;
        tmp=gpio[GPLEV0/4]&(1<<17);
        if(tmp!=prev){
            if(tmp&(1<<17)){
                printf("switch on\n");
            }
            else printf("switch off\n");
        }
    }

    munmap(addr_gpio, BLOCK_SIZE);
    close(memfd);

    return 0;
}
