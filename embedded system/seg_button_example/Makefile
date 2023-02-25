obj-m += seg_driver.o
obj-m += button_driver.o
KDIR = ~/working/kernel
RESULT = seg_example
SRC = $(RESULT).c
RESULT2 = seg_example2
SRC2 = $(RESULT2).c
CCC = aarch64-linux-gnu-gcc
RESULT3 = seg_example3
SRC3 = $(RESULT3).c


all:
	make -C $(KDIR) M=$(PWD) modules
	$(CCC) -o $(RESULT) $(SRC)
	$(CCC) -o $(RESULT2) $(SRC2)
	$(CCC) -o $(RESULT3) $(SRC3)


clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f $(RESULT) $(RESULT2) $(RESULT3)
