using namespace std;

#ifndef _MIPSCODE_H
#define _MIPSCODE_H

#include <string>

extern ofstream mipsfile;
extern vector<string> constStrings;

extern QCODE midcode[MAXCODELEN];
extern int codeCnt;        //中间代码条数指针

#define isInt(x) (((x) >= '0' && (x) <= '9') || ((x) == '-'))   //判断x是否是整数的开头
#define isUint(x) ((x) >= '0' && (x) <= '9')   //判断x是否是无符号整数的开头

#define isTempVal(x) ((x)[0] == '$')        //判断x是否为中间代码临时变量

typedef struct
{
    string name;
    int address;
} Localvar;     //局部变量(标识符和地址)

int addLocalVar(string name, int size = 1); //添加局部变量，size表示数组长度

int findLocalVar(string name);

void genMipsCode();

void initMips();

void var_code();

void end_code();

void calc_code();

void func_code();

void scnf_code();

void prnt_code();

void ret_code();

void lab_code();

void para_code();

void parav_code();

void call_code();

void jne_code();

void jmp_code();

void relate_code();

void ass_code();

void assa_code();

void aass_code();

void halt_code();

void newline_code();

#endif
