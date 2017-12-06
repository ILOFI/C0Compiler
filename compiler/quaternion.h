#ifndef _QUATERNION_H
#define _QUATERNION_H

#include <string>

#define MAXCODELEN 1024

using namespace std;

enum oprSet {CONSTOP, VAROP, INTOP, CHAROP, VOIDOP, ENDOP, ADDOP, SUBOP, MULOP, DIVOP, FUNCOP, 
             SCNFOP, PRNTOP, RETOP, LABOP, PARAOP, PARAVOP, CALLOP, JNEOP, JMPOP,
             GREOP, LESOP, EQUOP, NEQOP, GEQOP, LEQOP, ASSOP, ASSAOP, AASSOP,
             SPACEOP};

const string oprstr[] = {
    "const", "var", "int", "char", "void", "end", "+", "-", "*", "/", "func",
    "scanf", "prntf", "ret", "lab", "para", "parav", "call", "jne", "jmp",
    ">", "<", "==", "!=", ">=", "<=", "=", "[]=", "=[]",
    " "
};

typedef struct
{   //四元式结构
    oprSet opr;                    //操作符
    string lvar, rvar, ret;     //左操作数、右操作数和结果
} QCODE;

QCODE midcode[MAXCODELEN];

extern ofstream midcodeout;    //中间代码输出文件

extern int codeCnt;        //中间代码条数指针
extern int labelCnt;       //标签数指针
extern int tvarCnt;        //临时变量个数
extern bool midcodeDbg;     //输出调试信息

void initQuaternion();      //初始化

string genNewLab();        //生成一个新标签

string genNewVar();        //生成一个新的临时变量

string int_to_str(int x);

void genQuaternion(oprSet opr, string lvar, string rvar, string ret);          //生成一条四元式

void genQuaternion(oprSet opr, string lvar, int rvar, string ret);             //生成一条四元式（右操作数为整型）

void checkReturnCode();                 //检查当前是否有一条ret指令，若没有手动添加一条

#endif
