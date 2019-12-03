#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define INPUT_BUFFER_SIZE 20

extern int errno;

uint32_t calc_fuel(uint32_t mass)
{
	return (mass / 3) - 2;
}

int make_fd(void)
{
	int fd = open("inputData/input.txt", O_RDONLY);

	if(fd == -1)
	{
		printf("Error Number % d\n", errno);
		return -1;
	}

	return fd;
}

int read_num_input(int fd, uint32_t *number)
{
	char buf[INPUT_BUFFER_SIZE];
	char byte_out[1];
	int bytes_read = 0;
	uint8_t idx = 0;

	while(bytes_read = read(fd, byte_out, 1) > 0)
	{
		buf[idx] = byte_out[0];
		/* If we have newline we have a full line */
		if(buf[idx] == 0x0A)
		{
			buf[idx] = '\0';
			break;
		}
		idx++;
	}

	if(bytes_read == 0)
	{
		buf[idx] = '\0';
	}

	if(bytes_read == -1)
	{
		printf("Error Number % d\n", errno);
		return -1;
	}

	if(bytes_read > (INPUT_BUFFER_SIZE - 1))
	{
		printf("Input too long!");
		return -1;
	}

	if(bytes_read == 0 && idx == 0)
	{
		return 0;
	}

	*number = atoi(buf);

	return 1;
}

int main(void) {
	uint32_t number = 0;
	uint32_t fuel_sum = 0;
	int fd = make_fd();

	if(fd == -1)
		return EXIT_FAILURE;

	while(read_num_input(fd, &number) == 1)
	{
		printf("Fuel: %d\n", calc_fuel(number));
		fuel_sum += calc_fuel(number);
		printf("Total fuel: %d\n", fuel_sum);
	}

	return EXIT_SUCCESS;
}
