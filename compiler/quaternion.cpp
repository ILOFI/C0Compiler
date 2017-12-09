#include "stdafx.h"

using namespace std;

void initQuaternion()
{
    codeCnt = 0;
    labelCnt = 0;
    tvarCnt = 0;
    constStrings.clear();
}

string genNewLab()
{
    char label[16];
    sprintf(label, "_label_%d", labelCnt++);
    return label;
}

string genNewVar()
{
    char var[16];
    sprintf(var, "$*t_%d", tvarCnt++);
    return var;
}

string getStringLab()
{
    //获得常量字符串当前标号: _string_$d
    char var[32];
    sprintf(var, "._string_%d", constStrings.size());
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
    if (opr != PRNTOP) transform(lvar.begin(), lvar.end(), lvar.begin(), ::tolower);
    transform(rvar.begin(), rvar.end(), rvar.begin(), ::tolower);
    transform(ret.begin(), ret.end(), ret.begin(), ::tolower);

    midcode[codeCnt].opr = opr;
    midcode[codeCnt].lvar = lvar;
    midcode[codeCnt].rvar = rvar;
    midcode[codeCnt].ret = ret;
    codeCnt++;

    if (midcodeDbg && opr == FUNCOP) cout << endl; 
    if (midcodeDbg) cout << oprstr[(int)opr] << ", " << lvar << ", " << rvar << ", " << ret << endl;
    if (midcodeDbg && opr == ENDOP) cout << endl; 
}

void genQuaternion(oprSet opr, string lvar, int rvar, string ret)             //生成一条四元式（右操作数为整型）
{
    if (opr != PRNTOP) transform(lvar.begin(), lvar.end(), lvar.begin(), ::tolower);
    transform(ret.begin(), ret.end(), ret.begin(), ::tolower);

    midcode[codeCnt].opr = opr;
    midcode[codeCnt].lvar = lvar;
    midcode[codeCnt].rvar = int_to_str(rvar);
    midcode[codeCnt].ret = ret;
    codeCnt++;

    if (midcodeDbg && opr == FUNCOP) cout << endl; 
    if (midcodeDbg) cout << oprstr[(int)opr] << ", " << lvar << ", " << int_to_str(rvar) << ", " << ret << endl;
    if (midcodeDbg && opr == ENDOP) cout << endl; 
}

void checkReturnCode(bool inMain)
{
    if (inMain)
    {
        if (midcode[codeCnt-1].opr != HALTOP)
            genQuaternion(HALTOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], oprstr[(int)SPACEOP]);
    }
    else 
    {
        if (midcode[codeCnt-1].opr != RETOP)
            genQuaternion(RETOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], oprstr[(int)SPACEOP]);
    }
}
