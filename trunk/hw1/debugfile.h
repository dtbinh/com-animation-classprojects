#ifndef _DEBUGFILE_H
#define _DEBUGFILE_H
#include <stdio.h>

class DebugFile
{

public:
	DebugFile(char *filename);
	~DebugFile();
	void flush();

	FILE *fp;
};

#endif
