#ifndef _OPTIMIZE_H
#define _OPTIMIZE_H

#include <string>

#define isExprOp(x) ((x) == ADDOP || (x) == SUBOP || (x) == MULOP || (x) == DIVOP || (x) == ASSOP || (x) == ASSAOP || (x) == AASSOP)

using namespace std;

typedef struct
{
    int id;                         //块id
    string label;                   //块标签/函数名
    vector<QCODE> midcodes;         //基本快内的语句序列
    vector<int> prev, next;         //块的前驱和后继们
} basicBlock;               //基本块

struct DAGNODE
{
    int id;
    string name;
    vector<int> parent;
    int left, right;
};

typedef struct DAGNODE DAGNode;

typedef struct
{
    string name;
    int id;
    bool init;
} DAGItem;

extern QCODE midcode[MAXCODELEN];

extern vector<basicBlock> basicblocks;			//基本块

void optimize();            //优化总函数

void constCombine();        //常数合并

void basicBlockPartition(); //基本块划分

void linktoBlock(int, int);

void linktoBlock(int, string);      //连接到指定基本块并建立反向连接

void basicBlockLink();      //基本块连接

void basicBlockPrintf(string);

void buildDAG(vector<QCODE>);       //根据四元式序列生成DAG图

void DAGPrintf();

void DAGOptimize(string filename = " ");

#endif
