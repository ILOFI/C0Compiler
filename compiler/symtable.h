#ifndef _SYMTABLE_H_
#define _SYMTABLE_H_

#include <string>
using namespace std;

#define TABSIZE 512
#define MAXLEV 256

enum symkind {VARKD, CONSTKD, FUNCKD, PARAKD};
enum symtype {INTTP, CHARTP, VOIDTP};

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
    int totSub;       //分程序总数
    int subpnt[MAXLEV]; //分程序索引表
} symTable;

extern symTable symbolTable;

symtype symbol_to_symtype(symbolSet x);
char * symkind_to_str(symkind x);
char * symtype_to_str(symtype x);

void initTable();

int insertTable(string name, symkind kind, symtype type, int value, int address, int len);

void updatePara(int para);

int searchTable(string name, bool findfunc);

void printSymTable();

#endif