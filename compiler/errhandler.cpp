#include "stdafx.h"

using namespace std;

void errmain()
{
    
}

void errmain(errType type, int lineCnt)
{
    printf("Error: At line No.%d, %s\n", lineCnt, errMsg[(int)type].c_str());
}
