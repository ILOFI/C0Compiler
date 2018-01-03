#include "stdafx.h"

using namespace std;

void errmain()
{
    printf("Error on line %d.\n", lineNum);
}

void errmain(errType type, int lineCnt)
{
    printf("Error on line %d: %s\n", lineCnt, errMsg[(int)type].c_str());
}

void errmain(errType type, int lineCnt, string errtoken)
{
	printf("Error on line %d: %s %s\n", lineCnt, errMsg[(int)type].c_str(), errtoken.c_str());
}
