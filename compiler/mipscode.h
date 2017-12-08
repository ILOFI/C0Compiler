using namespace std;

#ifndef _MIPSCODE_H
#define _MIPSCODE_H

#include <string>

extern ofstream mipsfile;
extern vector<string> constStrings;

extern QCODE midcode[MAXCODELEN];
extern int codeCnt;        //中间代码条数指针

#define isInt(x) (((x) >= '0' && (x) <= '9') || ((x) == '-'))   //判断x是否是整数的开头

typedef struct
{
    string name;
    int address;
} Localvar;     //局部变量(标识符和地址)

#endif
