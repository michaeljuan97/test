#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>

// MIC-720AI DI define
#define MIC_720AI_DI0		248
#define MIC_720AI_DI1		249
#define MIC_720AI_DI2		250
#define MIC_720AI_DI3		251

// MIC-720AI DO define
#define MIC_720AI_DO0		240
#define MIC_720AI_DO1		241
#define MIC_720AI_DO2		242
#define MIC_720AI_DO3		243


void gpio_setvalue(int gpio, int value)
{
	int fd = -1;
	char cmd[64];
	char dev[128];

	sprintf(cmd, "%d", gpio);
	fd = open("/sys/class/gpio/export", O_WRONLY | O_TRUNC);
	if (fd < 0)
	{
		perror("open device failed!");
		return;
	}
	write(fd, cmd, sizeof(cmd));
	close(fd);

	sprintf(cmd, "out");
 	sprintf(dev, "/sys/class/gpio/gpio%d/direction", gpio);
	fd = open(dev, O_WRONLY | O_TRUNC);
	if (fd < 0)
	{
		perror("open device failed!!");
		return;
	}
	write(fd, cmd, sizeof(cmd));
	close(fd);

	sprintf(cmd, "%d", value);
 	sprintf(dev, "/sys/class/gpio/gpio%d/value", gpio);
	fd = open(dev, O_WRONLY | O_TRUNC);
	if (fd < 0)
	{
		perror("open device failed!!!");
		return;
	}
	write(fd, cmd, sizeof(cmd));
	close(fd);
	
	sprintf(cmd, "%d", gpio);
	fd = open("/sys/class/gpio/unexport", O_WRONLY | O_TRUNC);
	if (fd < 0)
	{
		perror("open device unexport failed!!!!");
		return;
	}
	write(fd, cmd, sizeof(cmd));
	close(fd);
}

void gpio_getvalue(int gpio, int *value)
{
	int ret;
	int fd = -1;
	char cmd[64];
	char dev[128];

	sprintf(cmd, "%d", gpio);
	fd = open("/sys/class/gpio/export", O_WRONLY | O_TRUNC);
	if (fd < 0)
	{
		perror("open device failed!");
		return;
	}
	write(fd, cmd, sizeof(cmd));
	close(fd);

	sprintf(cmd, "in");
 	sprintf(dev, "/sys/class/gpio/gpio%d/direction", gpio);
	fd = open(dev, O_WRONLY | O_TRUNC);
	if (fd < 0)
	{
		perror("open device failed!!");
		return;
	}
	write(fd, cmd, sizeof(cmd));
	close(fd);

 	sprintf(dev, "/sys/class/gpio/gpio%d/value", gpio);
	fd = open(dev, O_RDONLY | O_TRUNC);
	if (fd < 0)
	{
		perror("open device failed!!!");
		return;
	}
	ret = read(fd, cmd, sizeof(cmd));
	if (ret < 0)
	{
		perror("read device failed!");
	}
	else
	{
		*value = atoi(cmd);
	}
	close(fd);
	
	sprintf(cmd, "%d", gpio);
	fd = open("/sys/class/gpio/unexport", O_WRONLY | O_TRUNC);
	if (fd < 0)
	{
		perror("open device unexport failed!!!!");
		return;
	}
	write(fd, cmd, sizeof(cmd));
	close(fd);
}

int main(int argc, char **argv)
{
	int	chanel, value;

    if (argc > 1) 
    {
		if (!strncasecmp(argv[1], "-set", 4)) 
		{
      		if (argc < 4)
      		{ 
         		fprintf(stderr, "The input is invalid!\n");
   				return -1;
      		}

      		chanel = atoi(argv[2]);
			if ((chanel > 3) || (chanel < 0))
			{
        		printf("DO chanel is 0-3\n");
        		return -1;
			}
			chanel = MIC_720AI_DO0 + chanel;

   			value = atoi(argv[3]);  	   	
			if ((value > 1) || (value < 0))
			{
        		printf("DO value is 0 or 1\n");
        		return -1;
			}
			// MIC-720AI DO output is Anti-logic
			if (value)
				gpio_setvalue(chanel, 0);
			else
				gpio_setvalue(chanel, 1);

			printf("DO chanel %d is set %d\n", chanel - MIC_720AI_DO0, value);
		} 
		else if (!strncasecmp(argv[1], "-get", 4)) 
		{
      		if (argc < 3)
      		{ 
         		fprintf(stderr, "The input is invalid!\n");
   				return -1;
      		}

      		int chanel = atoi(argv[2]);
			if ((chanel > 3) || (chanel < 0))
			{
        		printf("DI chanel is 0-3\n");
        		return -1;
			}

			chanel = MIC_720AI_DI0 + chanel;
			gpio_getvalue(chanel, &value);
			printf("DI chanel %d value is %d\n", chanel - MIC_720AI_DI0, value);
	    }
    } 
    else 
    {
   		printf("Usage: %s [OPTIONS] DI/DO_chanel [value]\n", argv[0]);
   		printf("OPTIONS:\n");
   		printf(" -set: set DO value\n");
   		printf(" -get: get DI value\n");
   		printf("DI/DO_chanel:\n");
   		printf(" DI: 0-3, DO: 0-3\n");
   		printf("value:\n");
   		printf(" 0 or 1, only for DO\n");
   		return -1;
    }

  	return 0;
}
