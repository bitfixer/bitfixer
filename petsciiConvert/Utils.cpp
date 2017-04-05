/*
 *  Utils.cpp
 *  gamester_iphone
 *
 *  Created by Michael Hill on 10/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Utils.h"

bool within_range(int from_x, int from_y, int to_x, int to_y, float distance)
{
	float this_dist;
	int x_diff, y_diff;
	
	x_diff = to_x - from_x;
	y_diff = to_y - from_y;
	
	this_dist = sqrt((float)(x_diff*x_diff + y_diff*y_diff));
	if (this_dist <= distance)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int my_round(float f)
{
	float rem;
	int w;
	w = floor(f);
	rem = f - (float)w;

	if (rem >= 0.5)
	{
		return w+1;
	}
	else
	{
		return w;
	}
}

void trim(char *string)
{
	// find start
	char trimmed[200];
	int start, end, numchars;
	int i;
	char tmp;
	for (i = 0; i < strlen(string); i++)
	{
		tmp = string[i];
		start = i;
		if (!iswhitespace(tmp))
		{
			break;
		}
	}
	
	//printf("start is: %d\n", start);
	
	// find end
	for (i = strlen(string)-1; i >= 0; i--)
	{
		tmp = string[i];
		end = i;
		if (!iswhitespace(tmp))
		{
			break;
		}
	}
	
	//printf("end is: %d\n", end);
	
	// copy the trimmed string to a temp buffer
	numchars = end-start+1;
	strncpy(trimmed, &string[start], numchars);
	trimmed[numchars] = 0;
	
	// copy it back
	strcpy(string, trimmed);
}

bool iswhitespace(char t)
{
	if (t == ' ' || t == '\t' || t == '\n' || t == '\v' || t == '\f' || t == '\r')
	{
		return true;
	}
	else
	{
		return false;
	}
}

void *my_malloc(size_t size)
{
	void *res;
	res = malloc(size);
	if (res == NULL)
	{
		// failed to allocate memory
		// report the failure and die
		printf("malloc failed trying to allocate %d bytes\n", size);
		exit(0);
	}
	
	return res;
}

void debug(char *str)
{
	//printf("%s\n", str);
}

void str_replace(char *haystack, char *needle, char *replace)
{
	char *tmp;
	char result[1000];
	bool started;
	sprintf(result, "");
	started = false;
	tmp = strtok(haystack, needle);
	while (tmp)
	{
		if (started)
		{
			// append the needle replacement
			strcat(result, replace);
		}
		else
		{
			started = true;
		}
		
		// append the latest result
		strcat(result, tmp);
		
		tmp = strtok(NULL, needle);
	}
	
	strcpy(haystack, result);
}

void text_url_escape(char *str)
{
	str_replace(str, " ", "%20");
	str_replace(str, "\n", "%0D%0A");
}


