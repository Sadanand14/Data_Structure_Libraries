#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

struct Route
{
	int weight;
	int cityID1, cityID2;
};

struct City
{
	int ID;
	int minMessageTime;
	struct Route** routeArray;
};


void SortRoutes(struct Route** routes, unsigned int size);
void SortCities(struct City* cities, int* cityIDs, unsigned int size);
void RunCityMessengerTask();
void PopulateRoutes(struct Route* routes, unsigned int size);
int FastestTimeAlgo(struct Route* routes, unsigned int size);
//void PopulateCities(struct City* cities, unsigned int size);
//void DePopulateCities(struct City* cities, unsigned int size);



//void main()
//{
//	RunCityMessengerTask();
//}

//Assuming that path weights are always non-negative
void RunCityMessengerTask()
{
	printf("Please provide input for adjacenecy matrix \n");

	int input = 0;
	scanf_s("%d", &input);

	//routeArray containing bidirectional routes 
	struct Route* routeArray = malloc(((input * (input + 1)) / 2) * sizeof(struct Route));

	//function that takes input size and matrix inputs to populate the supplied routes with appropriate destination ends and weights.
	PopulateRoutes(routeArray, input);

	printf("\nFastest time for the last city to get the message : %d\n", FastestTimeAlgo(routeArray, input));

	free(routeArray);
}

void PopulateRoutes(struct Route* routes, unsigned int size)
{
	const char* delimitor = " ";

	//formula for index iteration across the lower triangle and diagonal only based on City ID: i *(i+1)/2 + j ; 
	/*(0,0 -> 0)
	(1, 0 -> 1) (1, 1 -> 2)
	(2, 0 -> 3) (2, 1 -> 4) (2, 2 -> 5) ...*/

	for (unsigned int i = 0; i < size; ++i)
	{
		int index = (i * (i + 1)) / 2 + i;
		routes[index].weight = 0;
		routes[index].cityID1 = i;
		routes[index].cityID2 = i;

		if (i < 1) continue;
		const char line[100];
		rsize_t strMax = sizeof(line);

		for (unsigned int j = 0; j < i; ++j)
		{
			scanf("%s", line);
			if ((strcmp(line, "x") && strcmp(line, "X")))
			{
				int val = atoi(line);

				index = (i * (i + 1)) / 2 + j;
				routes[index].weight = val;
				routes[index].cityID1 = i;
				routes[index].cityID2 = j;
			}
			else
			{

				index = (i * (i + 1)) / 2 + j;
				routes[index].weight = INT_MAX;
				routes[index].cityID1 = i;
				routes[index].cityID2 = j;
			}

		}
	}

	/*printf("matrix values: \n");

	for (unsigned int i = 0; i < size; ++i)
	{
		for (unsigned int j = 0; j <= i; ++j)
		{
			printf("%d\t", routes[(i * (i+1))/2 + j].weight);
		}
		printf("\n");
	}*/
}


//insertion sort on cities according to their current minMessageTimes
//I chose insertion sort simply because it has a fast best-case performance
//which would be the case with sorting here as not a lot would be changing per-sort.
void SortCities(struct City* cities, int* cityIDs, unsigned int size)
{
	struct City key;
	unsigned int i;
	int j;
	for (i = 1; i < size; ++i)
	{
		key = cities[i];
		j = i - 1;
		while (j >= 0 && cities[j].minMessageTime > key.minMessageTime)
		{
			cities[j + 1] = cities[j];
			//adjusting the ID array to reflect the updated position of the city in the cities array.
			cityIDs[cities[j].ID] = j + 1;
			j = j - 1;
		}
		cities[j + 1] = key;

		//adjusting the ID array to reflect the updated position of the city in the cities array.
		cityIDs[key.ID] = j + 1;
	}

	////refresh ID array pointing to the index of the city in the cities array
	//for (unsigned int i = 0; i < size; ++i) 
	//{
	//	cityIDs[cities[i].ID] = i;
	//}
}

//insertion sort for sorting a route array of a particular city based on their weights
//this one doesnt get used in this problem
void SortRoutes(struct Route** routes, unsigned int size)
{
	struct Route* key;
	unsigned int i;
	int j;

	for (i = 1; i < size; ++i)
	{
		key = routes[i];
		j = i - 1;
		while (j >= 0 && routes[j]->weight > key->weight)
		{
			routes[j + 1] = routes[j];
			j = j - 1;
		}
		routes[j + 1] = key;
	}
}

int FastestTimeAlgo(struct Route* routes, unsigned int size)
{
	struct City* cities = malloc(size * sizeof(struct City));

	//id array saves us time of looking through cities array by being a lookup table for their positions in the cities array after sorting.
	int* cityIDArray = malloc(size * sizeof(int));

	//populate city structs
	{
		int count;

		for (unsigned int i = 0; i < size; ++i)
		{
			cities[i].ID = i;
			cityIDArray[i] = (int)i;
			cities[i].routeArray = malloc(size * sizeof(struct Route*));
			count = 0;
			for (unsigned int j = 0; j < (size * (size + 1)) / 2; ++j)
			{
				if (routes[j].cityID1 == i)
				{
					cities[i].routeArray[count++] = &routes[j];
					if (routes[j].cityID2 == 0)
						cities[i].minMessageTime = routes[j].weight;
				}
				else if (routes[j].cityID2 == i)
				{
					cities[i].routeArray[count++] = &routes[j];
					//just for the one time of (0,0)
					if (routes[j].cityID1 == 0)
						cities[i].minMessageTime = routes[j].weight;
				}
			}
			//check to make sure that all cities have exactly one route to itself as well as each of the other cities 
			// even if their weights are extemely high indicating impassable.
			//for shipping code this should proably throw an exception which gets handled immediately.
			assert(count == size);
		}
	}


	printf("\n List of route times \n");
	for (unsigned int i = 0; i < size; ++i)
	{
		for (unsigned int j = 0; j < size; ++j)
			printf("%d\t", cities[i].routeArray[j]->weight);

		printf("\n");
	}

	SortCities(cities, cityIDArray, size);

	int value1, value2;
	//go through each city other than first/source city using Dijkstra's algorithm
	//consider shortest message time city and using sorting and recalculate paths to all connected cities via this city.
	//move to consider the next shortest time city based on updated calculations through sorting.
	for (unsigned int i = 1; i < size; ++i)
	{
		for (unsigned int j = 0; j < size; ++j)
		{
			//this branching comes at the at the cost of a bit of performance but was needed to 
			//reduce memory of routes array by a little less than half

			//we're checking if city1 of this route is the current city under consideration
			if (cities[i].routeArray[j]->cityID1 == cities[i].ID)
			{
				//check if the city connected through the current route to the current city has a shorter time value already than
				//a path via current city, if not then replace the value with the shorter path time through current city
				value1 = abs(cities[cityIDArray[cities[i].routeArray[j]->cityID2]].minMessageTime);
				value2 = cities[i].minMessageTime + cities[i].routeArray[j]->weight;

				//due to addition with INT_MAX the value turns negative as the last bit changes to 1
				//so we much check for that.
				if (value2 < 0)
					value2 = INT_MAX;

				//replace value if shorter time path is available
				if (value1 > value2)
					cities[cityIDArray[cities[i].routeArray[j]->cityID2]].minMessageTime = value2;
			}
			else
			{
				//check if the city connected through the current route to the current city has a shorter time value already than
				//a path via current city, if not then replace the value with the shorter path time through current city
				value1 = abs(cities[cityIDArray[cities[i].routeArray[j]->cityID1]].minMessageTime);
				value2 = cities[i].minMessageTime + cities[i].routeArray[j]->weight;

				//due to addition with INT_MAX the value turns negative as the last bit changes to 1
				//so we much check for that.
				if (value2 < 0)
					value2 = INT_MAX;

				//replace value if shorter time path is available
				if (value1 > value2)
					cities[cityIDArray[cities[i].routeArray[j]->cityID1]].minMessageTime = value2;
			}
		}
		//sorting is necessary to keep the order in the for loop
		SortCities(cities, cityIDArray, size);
	}

	int maxTime = 0;
	printf("\nFinal List of message times \n");
	for (unsigned int i = 0; i < size; ++i)
	{
		printf("City %d : %d\n", cities[i].ID, cities[i].minMessageTime);
	}

	for (unsigned int i = 0; i < size; ++i)
	{
		if (maxTime < cities[i].minMessageTime)
			maxTime = cities[i].minMessageTime;
	}

	for (unsigned int i = 0; i < size; ++i)
	{
		free(cities[i].routeArray);
	}

	free(cityIDArray);
	free(cities);
	return maxTime;
	//return 0;
}