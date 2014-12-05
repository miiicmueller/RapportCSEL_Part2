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

int main()
{
	int fd;
	uint8_t	button_state=0;	/* initialize the push-button former state */

	/* open memory file descriptor */
	fd = open ("/dev/mem", O_RDWR);
	if (fd < 0) {
	 	perror("open /dev/mem:"); 
		return -1;
	}

	/* map to user space APF27 FPGA registers */
	gpio = mmap (0, 256, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xd6000000);
	if (gpio==(void *)-1) { 
		printf("mmap failed, error: %i:%s \n",errno, strerror(errno)); 
		return -1;
	}
	
	while (1){
		button_state = ~(gpio->leds_rw >> 8) & 0x7;
		printf("result =0x%02x\n", button_state);
		usleep(500000);
	}

	munmap ((void*)gpio, 256);
	close (fd);
	return 0;
}

