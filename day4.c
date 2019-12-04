#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

uint32_t getNextNumber(uint32_t num)
{
	uint32_t res = 0;

	if(num == 0)
	{
		/* Stop condition */
		return num;
	}

	if(num % 10 == 9)
	{
		res = getNextNumber(num / 10);
		num = (res * 10) + (res % 10);
	}
	else
	{
		num++;
	}

	return num;
}

bool containsPair(uint32_t num)
{
	bool seeTwo = false;
	bool seeThree = false;
	bool seenThree = false;

	while(num/10 > 0)
	{
		/* Check if two digits match */
		seeTwo = ((num % 10) == ((num / 10) % 10));
		seeThree = seeTwo && ((num % 10) == ((num / 100) % 10));

		//if(seeTwo) /* For part 1 */
		/* Check if two digits match, but not three */
		if(seeTwo && !seeThree && !seenThree) /* For part 2 */
		{
			return true;
		}

		seenThree = seeThree; /* Save variable from last iteration */
		num /= 10;
	}
	return false;
}

int main(void) {
	uint32_t num = 231832;	/* Input */
	uint32_t numEnd = 767346;
	num = 233333;			/* Modified input */
	uint32_t cnt = 0;

	printf("Range: %d\n", numEnd - num);

	while(num <= numEnd)
	{
		num = getNextNumber(num);

		if(containsPair(num))
		{
			printf("Number: %d\n", num);
			cnt++;
		}
	}

	printf("Count: %d\n", cnt);

	printf("Done");

	exit(EXIT_SUCCESS);
}
