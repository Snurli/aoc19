#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define MAX_PARAMS 3

typedef enum Cmd
{
	ADD = 1,
	MULTIPLY = 2,
	INPUT = 3,
	OUTPUT = 4,
	JUMP_IF_TRUE = 5,
	JUMP_IF_FALSE = 6,
	LESS_THAN = 7,
	EQUALS = 8,
	ADJUST_RELATIVE_BASE = 9,
	HALT = 99
} Cmd;

typedef enum Mode
{
	POSITION = 0,
	IMMEDIATE = 1,
	RELATIVE = 2
} Mode;

uint32_t m_relativeBaseOffset = 0;

/* Prints array */
static void printData(int32_t data[], uint32_t length)
{
	uint32_t i = 0;

	for(i = 0; i < length; i++)
	{
		printf("data[%d] = %d\n", i, data[i]);
	}
}

/* Gives an immediate or position parameter depending on the mode */
static void getParam(int64_t *data, uint32_t idx, Mode mode, int64_t **paramOut)
{
	/* Get a pointer to immediate IMMEDIATE or POSITION */
	*paramOut = (mode == IMMEDIATE) ? &data[idx] : (mode == POSITION) ? &data[data[idx]] : &data[m_relativeBaseOffset + data[idx]];
}

/* Does the same as getParam, but for multiple parameters */
static void getParams(int64_t *data, uint32_t idx, uint8_t modes, int64_t *params[], uint8_t nParams)
{
	uint8_t i = 0;
	for(i = 0; i < nParams; i++)
	{
		getParam(data, idx + i, modes % 10, &params[i]);
		modes /= 10;
	}
}

/* Adds operant1 to operant2 and stores the result in storeAddr */
static void add(int64_t *operant1, int64_t *operant2, int64_t *storeAddr)
{
	*storeAddr = *operant1 + *operant2;
}

/* Multiplies operant1 to operant2 and stores the result in storeAddr */
static void mult(int64_t *operant1, int64_t *operant2, int64_t *storeAddr)
{
	*storeAddr = *operant1 * *operant2;
}

/* Takes an integer input from user and stores it in storeAddr */
static void input(int64_t *storeAddr)
{
	printf("Enter input:\n");
	scanf("%ld", storeAddr);
}

/* Writes value in storeAddr in terminal */
static void output(int64_t *storeAddr)
{
	printf("Output: %ld\n", *storeAddr);
}

/* Jumps to programAddress if value is NOT equal to zero. Does nothing if it is zero */
static void jumpIfTrue(int64_t *value, int64_t *programAddress, int64_t *programCounter)
{
	*programCounter = (*value != 0) ? *programAddress : *programCounter + 3;
}

/* Jumps to programAddress if value IS equal to zero. Does nothing if it is not zero */
static void jumpIfFalse(int64_t *value, int64_t *programAddress, int64_t *programCounter)
{
	*programCounter = (*value == 0) ? *programAddress : *programCounter + 3;
}

/* Sets storeAddr to 1 if parameter 1 is less than parameter 2 and zero if it isn't */
static void isLessThan(int64_t *param1, int64_t *param2, int64_t *storeAddr)
{
	*storeAddr = (*param1 < *param2) ? 1 : 0;
}

/* Sets storeAddr to 1 if parameter 1 is equal to parameter 2 and zero if it isn't */
static void isEqualTo(int64_t *param1, int64_t *param2, int64_t *storeAddr)
{
	*storeAddr = (*param1 == *param2) ? 1 : 0;
}

/* Jumps to programAddress if value is NOT equal to zero. Does nothing if it is zero */
static void adjustRelativeBase(int64_t *value)
{
	m_relativeBaseOffset += *value;
}

static void runProgram(int64_t *data, uint32_t length)
{
	Cmd cmd = 0;
	Mode mode = 0;
	int64_t pc = 0; /* Program Counter */
	int64_t *params[MAX_PARAMS];

	while(pc < length)
	{
		/* Take two lowest digits of data */
		cmd = data[pc] % 100;
		/* Take the higher digits of data */
		mode = data[pc] / 100;

		//printf("cmd: %d\n", cmd);

		switch (cmd)
		{
			case ADD:
				/* Fetch parameters from memory */
				getParams(data, pc + 1, mode, params, 3);

				/* Perform command */
				add(params[0], params[1], params[2]);

				/* Increment program counter */
				pc += 4;
				break;

			case MULTIPLY:
				/* Fetch parameters from memory */
				getParams(data, pc + 1, mode, params, 3);

				/* Perform command */
				mult(params[0], params[1], params[2]);

				/* Increment program counter */
				pc += 4;
				break;

			case INPUT:
				/* Fetch parameters from memory */
				getParams(data, pc + 1, mode, params, 1);

				/* Perform command */
				input(params[0]);

				/* Increment program counter */
				pc += 2;
				break;

			case OUTPUT:
				/* Fetch parameters from memory */
				getParams(data, pc + 1, mode, params, 1);

				/* Perform command */
				output(params[0]);

				/* Increment program counter */
				pc += 2;
				break;

			case JUMP_IF_TRUE:
				/* Fetch parameters from memory */
				getParams(data, pc + 1, mode, params, 2);

				/* Perform command */
				jumpIfTrue(params[0], params[1], &pc);

				/* Program counter is set in jumpIfTrue */
				break;

			case JUMP_IF_FALSE:
				/* Fetch parameters from memory */
				getParams(data, pc + 1, mode, params, 2);

				/* Perform command */
				jumpIfFalse(params[0], params[1], &pc);

				/* Program counter is set in jumpIfFalse */
				break;

			case LESS_THAN:
				/* Fetch parameters from memory */
				getParams(data, pc + 1, mode, params, 3);

				/* Perform command */
				isLessThan(params[0], params[1], params[2]);

				/* Increment program counter */
				pc += 4;
				break;

			case EQUALS:
				/* Fetch parameters from memory */
				getParams(data, pc + 1, mode, params, 3);

				/* Perform command */
				isEqualTo(params[0], params[1], params[2]);

				/* Increment program counter */
				pc += 4;
				break;

			case ADJUST_RELATIVE_BASE:
				/* Fetch parameters from memory */
				getParams(data, pc + 1, mode, params, 3);

				/* Perform command */
				adjustRelativeBase(params[0]);

				/* Increment program counter */
				pc += 2;
				break;

			case HALT:
				return; /* We finish program here */

			default:
				perror("Invalid command!");
				exit(EXIT_FAILURE);
				break;
		}
	}
}

static uint32_t readData(int64_t data[])
{
	int fd;
	char buf[20];
	char readBuf[1];
	int bytes_read = 0;
	uint8_t idxBuf = 0;
	uint32_t idxData = 0;

	if ((fd = open("inputdata/input.txt", O_RDONLY)) == -1) {
			perror("Error opening file");
			exit(EXIT_FAILURE);
	}

	while (bytes_read = read(fd, readBuf, 1) > 0) {
		if(readBuf[0] == '\n')
		{
				/* We have full number */
				buf[idxBuf] = '\0';
				data[idxData] = atol(buf);
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

int main(void) {
	int64_t dataIn[1000];
	uint64_t numbersRead = 0;

	numbersRead = readData(dataIn);

	runProgram(dataIn, numbersRead);

	printf("Done");

	exit(EXIT_SUCCESS);
}
