
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


/* A linked list point */
typedef struct Point {
    int x;
	int y;
    struct Point *next;
} Point_t;

uint32_t calcDistBetweenPoints(Point_t *p1, Point_t *p2)
{
	return abs(p1->x - p2->x) + abs(p1->y - p2->y);
}

uint32_t calcManhattanDist(Point_t *p)
{
	return abs(p->x) + abs(p->y);
}

Point_t calcIntersection(Point_t *p1, Point_t *p2)
{
	Point_t intersection;
	intersection.next = NULL;

	if(p1 == NULL || p2 == NULL || p1->next == NULL || p2->next == NULL)
	{
		perror("Invalid input!");
		exit(EXIT_FAILURE);
	}

	if(p1->x == p1->next->x && p2->y == p2->next->y)
	{
		intersection.x = p1->x;
		intersection.y = p2->y;
	}
	else if(p1->y == p1->next->y && p2->x == p2->next->x)
	{
		intersection.x = p2->x;
		intersection.y = p1->y;
	}
	else
	{
		perror("Lines not orthogonal!");
		exit(EXIT_FAILURE);
	}

	return intersection;
}

bool doLinesIntersect(Point_t *p1, Point_t *p2)
{
	if(p1 == NULL || p2 == NULL || p1->next == NULL || p2->next == NULL)
	{
		perror("Invalid input!");
		exit(EXIT_FAILURE);
	}

	return (((p1->x <= p2->next->x) && (p1->next->x >= p2->x)) || ((p1->x >= p2->next->x) && (p1->next->x <= p2->x)))
			&& (((p1->y <= p2->next->y) && (p1->next->y >= p2->y)) || ((p1->y >= p2->next->y) && (p1->next->y <= p2->y)));

}

static void calcNewPoint(Point_t *current, Point_t *next, char direction, uint32_t length)
{
	switch(direction)
	{
	case 'R':
		next->x = current->x + length;
		next->y = current->y;
		break;
	case 'L':
		next->x = current->x - length;
		next->y = current->y;
		break;
	case 'U':
		next->x = current->x;
		next->y = current->y + length;
		break;
	case 'D':
		next->x = current->x;
		next->y = current->y - length;
		break;
	default:
		perror("Unknown direction...");
		exit(EXIT_FAILURE);
		break;
	}
}

static void push(Point_t *head, char direction, uint32_t length)
{
	Point_t *current = head;

	while(current->next != NULL)
	{
		current = current->next;
	}

	/* We add new point */
	current->next = malloc(sizeof(Point_t));

	/* Calculate new point and store in x and y */
	calcNewPoint(current, current->next, direction, length);
}

static void readData(Point_t *firstHead, Point_t *secondHead)
{
	/* File descriptor */
	int fd;

	/* Input info */
	char direction;
	uint32_t length = 0;

	/* Buffering */
	char buf[10];
	char readBuf[1];
	int bytes_read = 0;
	uint8_t idxBuf = 0;

	/* List index */
	uint8_t idxLst = 0;

	if ((fd = open("../inputdata/input.txt", O_RDONLY)) == -1)
	{
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	while (bytes_read = read(fd, readBuf, 1) > 0)
	{
		if(readBuf[0] == ',' || readBuf[0] == '\n')
		{
			/* We have full input */
			buf[idxBuf] = '\0';

			/* Read out direction */
			direction = buf[0];
			buf[0] = '0';

			/* Read out length */
			length = atoi(buf);
			idxBuf = 0;

			/* Create point and add it to the list */
			if(idxLst == 0)
			{
				push(firstHead, direction, length);
			}
			else if(idxLst == 1)
			{
				push(secondHead, direction, length);
			}
			else
			{
				perror("No support for more than 2 lists...");
				exit(EXIT_FAILURE);
			}
			if(readBuf[0] == '\n')
			{
				idxLst++;
			}
		}
		else
		{
			buf[idxBuf] = readBuf[0];
			idxBuf++;
		}
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
}

static void createHead(Point_t **head)
{
  *head = malloc(sizeof(Point_t));
  if(*head == NULL)
  {
	  perror("Error making ptr");
	  exit(EXIT_FAILURE);
  }

  (*head)->x = 0;
  (*head)->y = 0;
  (*head)->next = NULL;
}

int main(void) {
	Point_t *firstHead;
	Point_t *secondHead;
	uint32_t minDist = 1000000; /* High number */
	uint32_t distFirst = 0;
	uint32_t distSecond = 0;
	uint32_t distToIntersection = 0;

	Point_t *firstCpy;
	Point_t *secondCpy;

	Point_t intersection;

	createHead(&firstHead);
	createHead(&secondHead);

	readData(firstHead, secondHead);

	firstCpy = firstHead;
	secondCpy = secondHead;

  /* Loop through linked lists */
	while(firstCpy->next != NULL)
	{
		while(secondCpy->next != NULL)
		{
			if(doLinesIntersect(firstCpy, secondCpy))
			{
				intersection = calcIntersection(firstCpy, secondCpy);
				distToIntersection = distFirst + distSecond + calcDistBetweenPoints(firstCpy, &intersection) + calcDistBetweenPoints(secondCpy, &intersection);
				if(distToIntersection > 0 && distToIntersection < minDist)
				{
					minDist = distToIntersection;
				}
			}
			distSecond += calcDistBetweenPoints(secondCpy, secondCpy->next);
			secondCpy = secondCpy->next;
		}
		distSecond = 0;
		secondCpy = secondHead;

		distFirst += calcDistBetweenPoints(firstCpy, firstCpy->next);
		firstCpy = firstCpy->next;
	}

	printf("Minimum distance: %d\n", minDist);

	printf("Done");

	exit(EXIT_SUCCESS);
}
