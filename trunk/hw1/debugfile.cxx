#include "debugfile.h"

DebugFile::DebugFile(char *filename)
{ 
	fp = fopen(filename, "wt");
}

DebugFile::~DebugFile() 
{ 
	fclose(fp);
}

void DebugFile::flush()
{
	fflush(fp);
}

