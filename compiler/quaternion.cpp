#include "stdafx.h"

using namespace std;

void initQuaternion()
{
    codeCnt = 0;
    labelCnt = 0;
    tvarCnt = 0;
}

string genNewLab()
{
    char label[16];
    sprintf(label, "_Label_%d", labelCnt++);
    return label;
}

string genNewVar()
{
    char var[16];
    sprintf(var, "_t_%d", tvarCnt++);
    return var;
}

string int_to_str(int x)
{
    char ret[16];
    sprintf(ret, "%d", x);
    return ret;
}

void genQuaternion(oprSet opr, string lvar, string rvar, string ret)          //生成一条四元式
{
    midcode[codeCnt].opr = opr;
    midcode[codeCnt].lvar = lvar;
    midcode[codeCnt].rvar = rvar;
    midcode[codeCnt].ret = ret;
    codeCnt++;

    if (midcodeDbg) cout << oprstr[(int)opr] << ", " << lvar << ", " << rvar << ", " << ret << endl;
}

void genQuaternion(oprSet opr, string lvar, int rvar, string ret)             //生成一条四元式（右操作数为整型）
{
    midcode[codeCnt].opr = opr;
    midcode[codeCnt].lvar = lvar;
    midcode[codeCnt].rvar = int_to_str(rvar);
    midcode[codeCnt].ret = ret;
    codeCnt++;

    if (midcodeDbg) cout << oprstr[(int)opr] << ", " << lvar << ", " << int_to_str(rvar) << ", " << ret << endl;
}

void checkReturnCode()
{
    if (midcode[codeCnt].opr != RETOP)
        genQuaternion(RETOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], oprstr[(int)SPACEOP]);
}
