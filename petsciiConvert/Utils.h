/*
 *  Utils.h
 *  gamester_iphone
 *
 *  Created by Michael Hill on 10/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include "types.h"
using namespace std;

bool within_range(int from_x, int from_y, int to_x, int to_y, float distance);
int my_round(float f);
void trim(char *string);
bool iswhitespace(char t);
void *my_malloc(size_t size);
void debug(char *str);
void text_url_escape(char *str);
void str_replace(char *haystack, char *needle, char *replace);

// -- some classes for convenient data storage
// key-indexed array

class KeyArray
{
public:
	KeyArray(){}
	~KeyArray(){}
	
	void add(char *key, char *val)
	{
		int index;
		string thekey, theval;
		thekey = key;
		theval = val;
		
		index = get_index(key);
		if (index == -1)
		{
			keys.push_back(thekey);
			values.push_back(theval);
		}
		else
		{
			values[index] = theval;
		}
	}
	
	const char *get(char *key)
	{
		int index;
		index = get_index(key);
		
		if (index > -1)
		{
			return values[index].c_str();
		}
		else
		{
			return NULL;
		}
	}
	
	const char *get(int index)
	{
		return values[index].c_str();
	}
	 
	int len()
	{
		return keys.size();
	}
	
	void clear()
	{
		keys.clear();
		values.clear();
	}
	
	
private:
	
	int get_index(char *key)
	{
		string thekey;
		thekey = key;
		
		// iterate through the key/string combos and see if we have this key
		int num_keys = len();
		int k;
		
		for (k = 0; k < num_keys; k++)
		{
			// is there a match for the key?
			if (thekey.compare(keys[k]) == 0)
			{
				// yes, this is the right key
				return k;
			}
		}
		
		return -1;
	}
	
	vector<string> keys;
	vector<string> values;
};


#endif