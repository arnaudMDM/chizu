#include <stdio.h>
#include <string.h>
#include <cassert>
//#include <cstdlib>
#include "File.h"

#define NB_COORD_ELTS 3200000

DWORD readFile(LPCTSTR name, Coord ** ppCoord, Limit * pLimit){
	HANDLE hFile = CreateFile(name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	*ppCoord = (Coord *)malloc(sizeof(Coord)* NB_COORD_ELTS);
	DWORD nbCoordElts = 0;

	CHAR*   buffer = (CHAR *)malloc(NB_COORD_ELTS * 25);
	DWORD nbBytesRead = 0;
	ReadFile(hFile, buffer, NB_COORD_ELTS * 25, &nbBytesRead, NULL);
	CHAR * pch;
	CHAR *next_token = NULL;
	pch = strtok_s(buffer, ",\n", &next_token);

	while (pch != NULL){
		FLOAT lon = strtof(pch, NULL);
		if (pLimit->xMinimum > lon)
			pLimit->xMinimum = lon;
		else if (pLimit->xMaximum < lon)
			pLimit->xMaximum = lon;
		pch = strtok_s(NULL, ",\r\n", &next_token);
		FLOAT lat = strtof(pch, NULL);
		if (pLimit->yMinimum > lat)
			pLimit->yMinimum = lat;
		else if (pLimit->yMaximum < lat)
			pLimit->yMaximum = lat;
		//OutputDebugString(str);
		pch = strtok_s(NULL, ",\r\n", &next_token);

		(*ppCoord)[nbCoordElts].longuitude = lon;
		(*ppCoord)[nbCoordElts].latitude = lat;

	
		nbCoordElts++;
	}

	CloseHandle(hFile);
	free(buffer);

	return nbCoordElts;
}