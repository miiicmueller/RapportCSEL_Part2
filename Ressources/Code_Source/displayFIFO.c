#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/errno.h>

#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

/**
 * 7-segment display and LED interface
 */
struct gpio_ctrl {
	uint16_t reserved1[(0x08-0x00)/2];
	uint16_t seg7_rw;
	uint16_t seg7_ctrl;
	uint16_t seg7_id;
	uint16_t reserved2[1];
	uint16_t leds_rw;
	uint16_t leds_ctrl;
	uint16_t leds_id;
};
static volatile struct gpio_ctrl* gpio = 0;
/* 7-segment: segment definition

           +-- seg A --+
           |           |
         seg F       seg B
           |           |
           +-- seg G --+
           |           |
         seg E       seg C
           |           |
           +-- seg D --+
*/

#define SEG_DOT 0x004
#define SEG_A 0x008
#define SEG_B 0x010
#define SEG_C 0x020
#define SEG_D 0x040
#define SEG_E 0x080
#define SEG_F 0x100
#define SEG_G 0x200
static const uint16_t seg_7 [] =
	{
	   SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F	,	/* 0 */
		   SEG_B + SEG_C        			,	/* 1 */
  	   SEG_A + SEG_B                 + SEG_E + SEG_D + SEG_G,	/* 2 */
    	   SEG_A + SEG_B + SEG_C + SEG_D                 + SEG_G,	/* 3 */
    	           SEG_B + SEG_C                 + SEG_F + SEG_G,	/* 4 */
    	   SEG_A         + SEG_C + SEG_D         + SEG_F + SEG_G,	/* 5 */
    	   SEG_A         + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,	/* 6 */
    	   SEG_A + SEG_B + SEG_C				,	/* 7 */
    	   SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,	/* 8 */
    	   SEG_A + SEG_B + SEG_C + SEG_D + SEG_F + SEG_G	,	/* 9 */
    	   SEG_A + SEG_B + SEG_C + SEG_E + SEG_F + SEG_G	,	/* A */
     	                   SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,	/* b */
     	   SEG_A                 + SEG_D + SEG_E + SEG_F        ,	/* C */
     	           SEG_B + SEG_C + SEG_D + SEG_E +         SEG_G,	/* d */
     	   SEG_A                 + SEG_D + SEG_E + SEG_F + SEG_G,	/* E */
     	   SEG_A                         + SEG_E + SEG_F + SEG_G,	/* F */
	};
/**
 * Method to initialize the 7-segment display
 */
static void seg7_init()
{
	static bool is_initialized=false;
	if (!is_initialized)
	{
		gpio->leds_ctrl = 0xff;
		gpio->leds_rw = 0;
		gpio->seg7_ctrl = 0x3ff;
		gpio->seg7_rw = 0;
		is_initialized = true;
	}
}


/**
 * Method to display a decimal number on the 7-segment
 */
static void seg7_display (int8_t value)
{
	static int affno = 0;

	seg7_init();
	uint16_t dot = 0;

	if (value < 0)
	{
		value = -value;
		dot = SEG_DOT;
	}

	if(affno==0)
	{
		gpio->seg7_rw = seg_7[value % 10] + 0x1 + dot;
		affno = 1;
	}
	else
	{
		gpio->seg7_rw = seg_7[value / 10] + 0x2 + dot;
		affno = 0;
	}
}

//variable de la valeur du compteur
static int gCounter = 0;

//handler pour le signal SIGALRM pour rafraîchir l'affichage 7 seg (prototype)
static void sigALRM_handler(int signo);

//sleep sécurisé pour le cas ou il est interrompu trop tôt par SIGALRM (prototype)
void safeSleep(int ns);

//définition de l'emplacement du fichier FIFO
#define FIFO_NAME "/tmp/counterFIFO"

//définition de la longueur de la variable pour lire le fifo
#define STR_FIFO_LENGTH 50

int main()
{
	int fd;
	fd=open("/dev/mem", O_RDWR);
	if(fd<0) {printf("Could not open /dev/mem: error=%i\n", fd); return fd;}
	gpio = mmap(0, 256, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xd6000000);
	if(gpio==(void *)0xFFFFFFFF){printf("mmap failed, error: %i:%s \n",errno,strerror(errno)); return(-1);}

	//init du handler de SIGALRM
	if(signal(SIGALRM, sigALRM_handler) == SIG_ERR)
	{
		perror("SIGALRM handler");
		exit(EXIT_FAILURE);
	}

	//création d'un itimer pour générer le SIGALRM
	struct itimerval aItimerval;
	aItimerval.it_interval.tv_sec = 0;
	aItimerval.it_interval.tv_usec = 10000;
	aItimerval.it_value.tv_sec = 0;
	aItimerval.it_value.tv_usec = 10000;
	if(setitimer(ITIMER_REAL, &aItimerval, NULL) < 0)
	{
		perror("setitimer");
		exit(EXIT_FAILURE);
	}
int main()
{
	// ....
	// FPGA ....
	// Initialisation du itimer
	// ....
	//création d'un FIFO
	int fd_mk_fifo;
	fd_mk_fifo = mkfifo(FIFO_NAME, 0666);

	if(fd_mk_fifo < 0)
	{
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}

	//ouverture du FIFO en lecture
	int fd_open_fifo;
	fd_open_fifo = open(FIFO_NAME, O_RDONLY);
	if(fd_open_fifo <= 0)
	{
		perror("openFIFO");
		exit(EXIT_FAILURE);
	}


	while(1)
	{

		//lecture du FIFO
		char str_fifo[STR_FIFO_LENGTH];
		int n;

		//sleep pour économiser le temps processeur
		// car la fonction "read" s'est montrée non bloquante...
		safeSleep(10000000);

		n = read(fd_open_fifo, str_fifo, STR_FIFO_LENGTH);
		str_fifo[n]='\0';


		//analyse du contenu
		if(strcmp(str_fifo, "PLUS\n") == 0)
		{

			if(gCounter == 99)
				gCounter = 0;
			else gCounter++;
		}else if(strcmp(str_fifo, "MINUS\n") == 0)
		{

			if(gCounter == 0)
				gCounter = 99;
			else gCounter--;
		}else if(strcmp(str_fifo, "NULL\n") == 0)
		{

			gCounter = 0;
		}



	}
	return 0;
}

//handler pour le signal SIGALRM pour rafraîchir l'affichage 7 seg (définition)
static void sigALRM_handler(int signo)
{
	seg7_display (gCounter);
	return;
}

//sleep sécurisé pour le cas ou il est interrompu trop tôt par SIGALRM (définition)
void safeSleep(int ns)
{
	int ret;
	struct timespec req, rem;
	req.tv_sec = 0;
	req.tv_nsec = ns;
	while(1)
	{
		ret = nanosleep(&req, &rem);
		if(ret)
		{
			if(errno == EINTR)
			{
				req.tv_sec = rem.tv_sec;
				req.tv_nsec = rem.tv_nsec;
			}
			else perror("nanosleep");
		}
		else return;
	}
}






