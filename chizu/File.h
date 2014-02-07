#include "windows.h"

#ifndef FILE_H_
#define FILE_H_

struct Coord{
	float longuitude;
	float latitude;
	//Coord(float lat, float lon) :latitude(lat), longuitude(lon){}
};

struct Limit{
	float xMinimum;
	float xMaximum;
	float yMinimum;
	float yMaximum;
	Limit() :xMinimum(200.0f), xMaximum(-200.0f), yMinimum(200.0f), yMaximum(-200.0f){}
};
int readFile(LPCTSTR name, Coord ** ppCoord, Limit * pLimit);

#endif