#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define PLANET_NAME_MAX_LENGTH 4

/* A linked list point */
typedef struct Planet {
    char name[PLANET_NAME_MAX_LENGTH];
    struct Planet *child;
    struct Planet *sibling;
} Planet_t;

static bool findPlanet(Planet_t *planet, char name[], Planet_t **planetOut)
{
	if(planet == NULL)
	{
		return false;
	}

	/* Check if name matches */
	if(strcmp(planet->name, name) == 0)
	{
		*planetOut = planet;
		return true;
	}

	/* If planet is found for sibling, take identity */
	if(findPlanet(planet->sibling, name, planetOut))
	{
		return true;
	}

	/* If planet is found for child, take identity */
	if(findPlanet(planet->child, name, planetOut))
	{
		return true;
	}

	return false;
}

static void createSibling(Planet_t *parrent, char buf[], Planet_t **planetOut)
{
	Planet_t *current = parrent;

	while(current->sibling != NULL)
	{
		current = current->sibling;
	}

	/* We add new point */
	current->sibling = malloc(sizeof(Planet_t));
	current->sibling->name[0] = buf[0];
	current->sibling->name[1] = buf[1];
	current->sibling->name[2] = buf[2];
	current->sibling->name[3] = buf[3];
	*planetOut = current->sibling;
}

static void createChild(Planet_t *parrent, char buf[])
{
	Planet_t *current = parrent;

	if(current->child == NULL)
	{
		/* We add new point */
		current->child = malloc(sizeof(Planet_t));
		current->child->name[0] = buf[0];
		current->child->name[1] = buf[1];
		current->child->name[2] = buf[2];
		current->child->name[3] = buf[3];
		return;
	}

	current = current->child;

	while(current->sibling != NULL)
	{
		current = current->sibling;
	}

	/* We add new point */
	current->sibling = malloc(sizeof(Planet_t));
	current->sibling->name[0] = buf[0];
	current->sibling->name[1] = buf[1];
	current->sibling->name[2] = buf[2];
	current->sibling->name[3] = buf[3];
}

static void insertChild(Planet_t **parrent, Planet_t **child)
{
	if((*parrent)->child == NULL)
	{
		/* We add new point */
		(*parrent)->child = *child;
		return;
	}

	(*parrent) = (*parrent)->child;

	while((*parrent)->sibling != NULL)
	{
		(*parrent) = (*parrent)->sibling;
	}

	(*parrent)->sibling = *child;
}

static void readData(Planet_t *com)
{
	/* File descriptor */
	int fd;

	/* Buffering */
	char buf[4];
	char readBuf[1];
	int bytes_read = 0;
	uint8_t idxBuf = 0;
	uint16_t cnt = 0;

	Planet_t *parentPlanetOut = NULL;
	Planet_t *childPlanetOut = NULL;

	if ((fd = open("inputdata/input3.txt", O_RDONLY)) == -1)
	{
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	while (bytes_read = read(fd, readBuf, 1) > 0)
	{
		/* We have first planet (parent) */
		if(readBuf[0] == ')')
		{
			/* We have full input */
			buf[idxBuf] = '\0';

			if(!findPlanet(com, buf, &parentPlanetOut))
			{
				createSibling(com, buf, &parentPlanetOut);
			}

			idxBuf = 0;
		}
		/* We have second channel (child) */
		else if(readBuf[0] == '\n')
		{
			/* We have full input */
			buf[idxBuf] = '\0';

			if(findPlanet(com, buf, &childPlanetOut))
			{
				insertChild(&parentPlanetOut, &childPlanetOut);
			}
			else
			{
				createChild(parentPlanetOut, buf);
			}

			idxBuf = 0;
			printf("Count: %d\n", cnt);
			cnt++;
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

static void createCenterOfMass(Planet_t **com)
{
  *com = malloc(sizeof(Planet_t));
  if(*com == NULL)
  {
	  perror("Error making ptr");
	  exit(EXIT_FAILURE);
  }

  (*com)->name[0] = 'C';
  (*com)->name[1] = 'O';
  (*com)->name[2] = 'M';
  (*com)->name[3] = '\0';
}

int main(void) {
	Planet_t *com;

	createCenterOfMass(&com);

	readData(com);

	printf("Done");

	exit(EXIT_SUCCESS);
}
