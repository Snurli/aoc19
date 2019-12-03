#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

enum cmds
{
	ADD = 1,
	MULTIPLY = 2,
	HALT = 99
};

static void checkPrecondition(uint32_t data[], uint8_t idx, uint8_t length)
{
	/* Check that we have parameters and output position and that output position is within range*/
	if(!((idx + 3) < length))
	{
		perror("Too few parameters left!");
		exit(EXIT_FAILURE);
	}
	if(!(data[idx + 3] < length))
	{
		perror("Output is outside of range!");
		exit(EXIT_FAILURE);
	}
}

static void add(uint32_t data[], uint8_t idx)
{
	data[data[idx + 3]] = data[data[idx + 1]] + data[data[idx + 2]];
}

static void mult(uint32_t data[], uint8_t idx)
{
	data[data[idx + 3]] = data[data[idx + 1]] * data[data[idx + 2]];
}


static void runProgram(uint32_t data[], uint8_t length)
{
	uint32_t cmd = 0;
	uint8_t idx = 0;


	while(idx < length)
	{
		cmd = data[idx];

		switch (cmd)
		{
			case ADD:
				checkPrecondition(data, idx, length);
				add(data,idx);
				break;

			case MULTIPLY:
				checkPrecondition(data, idx, length);
				mult(data,idx);
				break;

			case HALT:
				return; /* We finish program here */

			default:
				perror("Invalid command!");
				exit(EXIT_FAILURE);
				break;
		}

		idx += 4;
	}
}

static uint8_t readData(uint32_t data[])
{
	int fd;
	char buf[20];
	char readBuf[1];
	int bytes_read = 0;
	uint8_t idxBuf = 0;
	uint8_t idxData = 0;

	if ((fd = open("../inputdata/input.txt", O_RDONLY)) == -1) {
			perror("Error opening file");
			exit(EXIT_FAILURE);
	}

	while (bytes_read = read(fd, readBuf, 1) > 0) {
		if(readBuf[0] == '\n')
		{
				/* We have full number */
				buf[idxBuf] = '\0';
				data[idxData] = atoi(buf);
				idxData++;
				idxBuf = 0;
		}

		buf[idxBuf] = readBuf[0];
		idxBuf++;
	}

	if(bytes_read == -1)
	{
		perror("Error reading from file");
		exit(EXIT_FAILURE);
	}

	if(close(fd) == -1)
	{
		perror("Error closing file");
		exit(EXIT_FAILURE);
	}

	return idxData;
}

static void printData(uint32_t data[], uint8_t length)
{
	uint8_t i = 0;

	for(i = 0; i < length; i++)
	{
		printf("data[%d] = %d\n", i, data[i]);
	}
}

static void copyData(uint32_t dataIn[], uint32_t dataOut[], uint8_t length)
{
	uint8_t i = 0;

	for(i = 0; i < length; i++)
	{
		dataOut[i] = dataIn[i];
	}
}

int main(void) {
	uint32_t dataIn[200];
	uint32_t dataCpy[200];
	uint8_t j = 0;
	uint8_t k = 0;
	uint8_t numbersRead = 0;

	numbersRead = readData(dataIn);

	for (j = 0; j < 100; j++) {
		for (k = 0; k < 100; k++) {

			//printData(dataCpy);

			copyData(dataIn, dataCpy, numbersRead);

			dataCpy[1] = j;
			dataCpy[2] = k;

			runProgram(dataCpy, numbersRead);

			if(dataCpy[0] == 19690720)
			{
				printf("Adr1 = %d, Adr2 = %d\n", j, k);
			}

			//printData(dataCpy);
		}
	}

	printf("Done");

	exit(EXIT_SUCCESS);
}
