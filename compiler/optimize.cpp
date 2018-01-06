#include "stdafx.h"

using namespace std;

void optimize()
{
    constCombine();
}

void constCombine()
{
    bool constfound;
    do
    {
        constfound = false;
        for (int i = 0; i < codeCnt; ++i)
            if ((midcode[i].opr == ADDOP || midcode[i].opr == SUBOP || midcode[i].opr == MULOP || midcode[i].opr == DIVOP) &&
                isInt(midcode[i].lvar[0]) && isInt(midcode[i].rvar[0]) && isTempVal(midcode[i].ret))
            {
                int tans;
                switch(midcode[i].opr)
                {
                    case ADDOP: tans = atoi(midcode[i].lvar.c_str()) + atoi(midcode[i].rvar.c_str());
                                break;

                    case SUBOP: tans = atoi(midcode[i].lvar.c_str()) - atoi(midcode[i].rvar.c_str());
                                break;

                    case MULOP: tans = atoi(midcode[i].lvar.c_str()) * atoi(midcode[i].rvar.c_str());
                                break;

                    case DIVOP: tans = atoi(midcode[i].lvar.c_str()) / atoi(midcode[i].rvar.c_str());
                                break;
                }

                string tvar = midcode[i].ret, tval = int_to_str(tans);
                midcode[i].opr = SPACEOP;
                midcode[i].lvar = " ";
                midcode[i].rvar = " ";
                midcode[i].ret = " ";

                constfound = false;
                for (int j = 0; j < codeCnt; ++j)
                {
                    if (midcode[j].lvar == tvar)
                    {
                        midcode[j].lvar = tval;
                        constfound = true;
                    }
                    if (midcode[j].rvar == tvar)
                    {
                        midcode[j].rvar = tval;
                        constfound = true;
                    }
                    if (midcode[j].ret == tvar)
                    {
                        midcode[j].ret = tval;
                        constfound = true;
                    }
                }
            }
    } while(constfound);
    
}

