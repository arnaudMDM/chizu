#include "windows.h"

#ifndef FILE_H_
#define FILE_H_

struct Coord{
	FLOAT longuitude;
	FLOAT latitude;
	//Coord(float lat, float lon) :latitude(lat), longuitude(lon){}
};

struct Limit{
	FLOAT xMinimum;
	FLOAT xMaximum;
	FLOAT yMinimum;
	FLOAT yMaximum;
	Limit() :xMinimum(200.0f), xMaximum(-200.0f), yMinimum(200.0f), yMaximum(-200.0f){}
};

DWORD readFile(LPCTSTR name, Coord ** ppCoord, Limit * pLimit);

#endif