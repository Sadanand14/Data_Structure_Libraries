//#include "SimpleDictionary.h"
#include "PreAllocated_SimpleDictionary.h"
#include <stdio.h>

void Testing(void*)
{
	printf("Testnig function runnning");
}

int main() 
{
	/*SimpleDictionary map;

	map.Add("Sadanand", nullptr, Testing);
	map.Add("Oxide", nullptr, Testing);
	map.Add("Games", nullptr, Testing);
	map.Add("Programming", nullptr, Testing);
	map.Add("RBTree", nullptr, Testing);
	map.Add("Gaming", nullptr, Testing);
	map.PrintAll();

	map.Remove("Gaming");*/


	PreAllocated_SimpleDictionary<const char* , int> map;

	map.Add("Sadanand", 5);
	map.Add("Oxide", 4);
	map.Add("Games", 1);
	map.Add("Programming", 7);
	map.Add("RBTree", 9);
	map.Add("Gaming", 10);
	map.PrintAll();

	map.Remove("Gaming");

	printf("\n%d", map["Games"]);
	return 1;
}