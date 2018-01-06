#ifndef _OPTIMIZE_H
#define _OPTIMIZE_H

#include <string>

using namespace std;

extern QCODE midcode[MAXCODELEN];

void optimize();            //优化总函数

void constCombine();        //常数合并

#endif
