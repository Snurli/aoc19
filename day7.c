#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

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
	HALT = 99
} Cmd;

typedef enum Mode
{
	POSITION = 0,
	IMMEDIATE = 1
} Mode;


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
static void getParam(int32_t *data, uint32_t idx, Mode mode, int32_t **paramOut)
{
	/* Get a pointer to immediate IMMEDIATE or POSITION */
	*paramOut = (mode == IMMEDIATE) ? &data[idx] : &data[data[idx]];
}

/* Does the same as getParam, but for multiple parameters */
static void getParams(int32_t *data, uint32_t idx, uint8_t modes, int32_t *params[], uint8_t nParams)
{
	uint8_t i = 0;
	for(i = 0; i < nParams; i++)
	{
		getParam(data, idx + i, modes % 10, &params[i]);
		modes /= 10;
	}
}

/* Adds operant1 to operant2 and stores the result in storeAddr */
static void add(int32_t *operant1, int32_t *operant2, int32_t *storeAddr)
{
	*storeAddr = *operant1 + *operant2;
}

/* Multiplies operant1 to operant2 and stores the result in storeAddr */
static void mult(int32_t *operant1, int32_t *operant2, int32_t *storeAddr)
{
	*storeAddr = *operant1 * *operant2;
}

/* Takes an integer input from user and stores it in storeAddr */
static void input(int32_t *storeAddr)
{
	printf("Enter input:\n");
	scanf("%d", storeAddr);
}

/* Writes value in storeAddr in terminal */
static void output(int32_t *storeAddr)
{
	printf("Output: %d\n", *storeAddr);
}

/* Jumps to programAddress if value is NOT equal to zero. Does nothing if it is zero */
static void jumpIfTrue(int32_t *value, int32_t *programAddress, uint32_t *programCounter)
{
	*programCounter = (*value != 0) ? *programAddress : *programCounter + 3;
}

/* Jumps to programAddress if value IS equal to zero. Does nothing if it is not zero */
static void jumpIfFalse(int32_t *value, int32_t *programAddress, uint32_t *programCounter)
{
	*programCounter = (*value == 0) ? *programAddress : *programCounter + 3;
}

/* Sets storeAddr to 1 if parameter 1 is less than parameter 2 and zero if it isn't */
static void isLessThan(int32_t *param1, int32_t *param2, int32_t *storeAddr)
{
	*storeAddr = (*param1 < *param2) ? 1 : 0;
}

/* Sets storeAddr to 1 if parameter 1 is equal to parameter 2 and zero if it isn't */
static void isEqualTo(int32_t *param1, int32_t *param2, int32_t *storeAddr)
{
	*storeAddr = (*param1 == *param2) ? 1 : 0;
}

static int32_t runProgram(int32_t *data, uint32_t length, uint8_t phaseSetting, int32_t inputValue)
{
	Cmd cmd = 0;
	Mode mode = 0;
	uint32_t pc = 0; /* Program Counter */
	int32_t *params[MAX_PARAMS];
	bool phaseSettingSet = false;
	static uint8_t phaseSettingSetCount = 0;

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
				//input(params[0]);

				if(!phaseSettingSet && !(phaseSettingSetCount >= 5))
				{
					*params[0] = phaseSetting;
					phaseSettingSet = true;
					phaseSettingSetCount++;
				}
				else
				{
					*params[0] = inputValue;
				}

				/* Increment program counter */
				pc += 2;
				break;

			case OUTPUT:
				/* Fetch parameters from memory */
				getParams(data, pc + 1, mode, params, 1);

				/* Perform command */
				//output(params[0]);

				return *params[0];

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

			case HALT:
				return; /* We finish program here */

			default:
				perror("Invalid command!");
				exit(EXIT_FAILURE);
				break;
		}
	}
}

static uint32_t readData(int32_t data[])
{
	int fd;
	char buf[20];
	char readBuf[1];
	int bytes_read = 0;
	uint8_t idxBuf = 0;
	uint32_t idxData = 0;

	if ((fd = open("inputdata/inputexample4.txt", O_RDONLY)) == -1) {
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

static uint32_t readInput(uint32_t data[])
{
	int fd;
	char buf[20];
	char readBuf[1];
	int bytes_read = 0;
	uint8_t idxBuf = 0;
	uint32_t idxData = 0;

	if ((fd = open("inputdata/uniq_permutations2.txt", O_RDONLY)) == -1) {
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

int main(void) {
	int32_t dataIn[550];
	uint32_t numbersRead = 0;
	uint32_t inputsRead = 0;
	uint32_t permutations[120];
	int32_t output = 0;
	int32_t input = 0;
	uint8_t phaseSetting = 0;
	uint8_t i = 0;
	int32_t maxValue = 0;
	uint32_t maxPermutation = 0;

	inputsRead = readInput(permutations);
	numbersRead = readData(dataIn);

	for (i = 0; i < 120; i++)
	{
		while(1)
		{
			/* Amplifier A */
			input = output;
			phaseSetting = permutations[i] % 10;
			output = runProgram(dataIn, numbersRead, phaseSetting, input);

			/* Amplifier B */
			input = output;
			phaseSetting = (permutations[i] / 10) % 10;
			output = runProgram(dataIn, numbersRead, phaseSetting, input);

			/* Amplifier C */
			input = output;
			phaseSetting = (permutations[i] / 100) % 10;
			output = runProgram(dataIn, numbersRead, phaseSetting, input);

			/* Amplifier D */
			input = output;
			phaseSetting = (permutations[i] / 1000) % 10;
			output = runProgram(dataIn, numbersRead, phaseSetting, input);

			/* Amplifier E */
			input = output;
			phaseSetting = (permutations[i] / 10000) % 10;
			output = runProgram(dataIn, numbersRead, phaseSetting, input);
		}
		/* Check max */
		if(maxValue < output)
		{
			maxValue = output;
			maxPermutation = permutations[i];
			printf("Phase settings (inversed): %d\n", maxPermutation);
		}
	}

	printf("Max value: %d\n", maxValue);
	printf("Phase settings (inversed): %d\n", maxPermutation);

	printf("Done");

	exit(EXIT_SUCCESS);
}
