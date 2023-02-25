#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

static struct termios init_setting, new_setting;
char seg_num[10] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90};
char seg_dnum[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x58, 0x00, 0x10};

#define D1 0x01
#define D2 0x02
#define D3 0x04
#define D4 0x08

char POSITION[4] = {D1, D2, D3, D4};

void init_keyboard()
{
	tcgetattr(STDIN_FILENO, &init_setting);
	new_setting = init_setting;
	new_setting.c_lflag &= ~ICANON;
	new_setting.c_lflag &= ~ECHO;
	new_setting.c_cc[VMIN] = 0;
	new_setting.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &new_setting);
}

void close_keyboard()
{
	tcsetattr(0, TCSANOW, &init_setting);
}

char get_key()
{
	char ch = -1;

	if(read(STDIN_FILENO, &ch, 1) != 1)
		ch = -1;
	return ch;
}

void print_menu()
{
	printf("\n----------menu----------\n");
	printf("[u] : count up\n");
    printf("[d] : count down\n");
    printf("[p] : count setting\n");
	printf("[q] : program exit\n");
	printf("------------------------\n\n");
}

// void display(int num, int dev);

int main(int argc, char **argv)
{
	unsigned short data;
	char key;
	int delay_time, time_for_display;

#define ON 49 //Button
#define OFF 48
	char prev[2] = {OFF, OFF};
	char buff[2];

#define UP 1 //Count
#define DOWN 0
	char up_down_flag = UP;

	int num = 0;
	int temp = 0;

	int dev = open("/dev/my_segment", O_RDWR); // if you want read='O_RDONLY' write='O_WRONLY', read&write='O_RDWR'
	int dev_button = open("/dev/my_button", O_RDONLY);

	if(dev == -1) {
		printf("Opening was not possible!\n");
		return -1;
	}

	if(dev_button == -1){
		printf("Opening_Button was not possible!\n");
		return -1;
	}

	printf("device opening was successfull!\n");

	init_keyboard();
	print_menu();
	delay_time = 1000000/100/4;

	// data[0] = (seg_num[1] << 4) | D1;
	// data[1] = (seg_num[2] << 4) | D2;
	// data[2] = (seg_num[3] << 4) | D3;
	// data[3] = (seg_num[4] << 4) | D4;

	while(1){
		/* DISPLAY NUMBER */
		for(int j=0; j<100; j++){
			read(dev_button, &buff, 2);

			/* UP, DOWN USING BUTTONS */
			if(buff[UP] == ON && prev[UP] == OFF){
				num++;
				if (num > 9999)	num =0;
			}

			else if(buff[DOWN] == ON && prev[DOWN] == OFF){
				num--;
				if (num < 0) num = 9999;
			}
			prev[UP] = buff[UP];
			prev[DOWN] = buff[DOWN];

			/* CONTROL USING KEYBOARD */
			key = get_key();
			if(key == 'q'){
				printf("exit this program.\n");
				close_keyboard();
				write(dev, 0x0000, 2);
				close(dev);
				return 0;
			}
			else if(key == 'u'){
				num++;
				if (num > 9999)	num =0;
			}
			else if(key == 'd'){
				num--;
				if (num < 0) num = 9999;
			}
			else if(key == 'p'){
				up_down_flag = !up_down_flag;
			}

			/* DISPLAY NUMBER */
			temp = num;
			for(int i=3;i>=0;i--){
				data = (seg_num[temp % 10] << 4) | POSITION[i];
				write(dev, &data,2);
				usleep(delay_time);
				temp = temp/10;
			}
		}
		
		/* NUMBER INCREMENT(DECRESEMENT) */
		if(up_down_flag == UP){
			num++;
			if (num > 9999)	num =0;
		}
		else if(up_down_flag == DOWN){
			num--;
			if (num < 0) num = 9999;
		}
			
	}	
	
}
