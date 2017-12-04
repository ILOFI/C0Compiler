#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <string>
using namespace std;

#define TABSIZE 1024
#define MAXLEV 512

enum symkind {VARKD, CONSTKD, PROCKD, FUNCKD};
enum symtype {INTTP, CHARTP};

typedef struct
{                   //符号表表项
    string name;    //标识符
    symkind kind;   //种类
    symtype type;   //变量类型或函数返回类型
    int value;      //常量值
    int address;    //存储地址或地址位移
    int len;        //数组长度或函数参数个数
} symTabItem;

typedef struct
{                       //符号表
    symTabItem item[TABSIZE];
    int curpnt;         //符号表当前位置指针
    int totLevel;       //分程序总数
    int levpnt[MAXLEV]; //分程序索引表
} symTable;

extern symTable symbolTable;

int insertTable(string name, symkind kind, symtype type, int value, int address, int len);

int searchTable(string name);

#endif