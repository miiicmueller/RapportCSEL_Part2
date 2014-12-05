#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/errno.h> 
#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>


struct gpio_ctrl {
	uint16_t res1[4];
	uint16_t seg7_rw;
	uint16_t seg7_ctrl;
	uint16_t seg7_id;
	uint16_t res2;
	uint16_t leds_rw;
	uint16_t leds_ctrl;
	uint16_t leds_id;
};

static volatile struct gpio_ctrl* gpio = 0;

//définition de l'emplacement du fichier FIFO
#define FIFO_NAME "/tmp/counterFIFO"

int main()
{
	int fd;
	uint8_t	button_state=0;	/* initialize the push-button former state */

	/* open memory file descriptor */
	fd = open ("/dev/mem", O_RDWR);
	if (fd < 0)
	{
	 	perror("open /dev/mem:"); 
		return -1;
	}

	/* map to user space APF27 FPGA registers */
	gpio = mmap (0, 256, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xd6000000);
	if (gpio==(void *)-1)
	{ 
		printf("mmap failed, error: %i:%s \n",errno, strerror(errno)); 
		return -1;
	}

int main()
{
	//.....
	// Stuff initialization
	// .....
	//ouverture du FIFO en lecture
	int fd_open_fifo;
	fd_open_fifo = open(FIFO_NAME, O_WRONLY);
	if(fd_open_fifo <= 0)
	{
		perror("openFIFO");
		exit(EXIT_FAILURE);
	}


	//variables des boutons
	int bt_plus, bt_moins, bt_null;
	
	while (1)
	{
		button_state = ~(gpio->leds_rw >> 8) & 0x7;
		//printf("result =0x%02x\n", button_state);
		usleep(500000);

		bt_plus = button_state & 0x01;
		bt_moins = button_state & 0x02;
		bt_null = button_state & 0x04;

		//écriture dans le FIFO
		if(bt_plus)
		{
			if(write(fd_open_fifo, "PLUS\n", 5) != 5)
			{
			perror("write"); 
			}
		}
		else if(bt_moins)
		{
			if(write(fd_open_fifo, "MINUS\n", 6) != 6)
			{
			perror("write"); 
			}
		}
		else if(bt_null)
		{
			if(write(fd_open_fifo, "NULL\n", 5) != 5)
			{
			perror("write"); 
			}
		}
	}

	munmap ((void*)gpio, 256);
	close (fd);
	return 0;
}

