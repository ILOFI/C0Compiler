#include "stdafx.h"

using namespace std;

void optimize()
{
    constCombine();
    basicBlockPartition();
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

void basicBlockPartition()
{
    bool entry[MAXCODELEN] = {0};
    
    basicblocks.clear();

    entry[0] = true;        //整个语句序列的第一条语句属于入口语句
    for (int i = 0; i < codeCnt; ++i)
    {
        //任何能由条件/无条件跳转语句转移到的第一条语句属于入口语句
        if (midcode[i].opr == FUNCOP || midcode[i].opr == LABOP)
            entry[i] = true;

        //紧跟在跳转语句之后的第一条语句属于入口语句
        if (midcode[i].opr == JNEOP || midcode[i].opr == JMPOP || midcode[i].opr == CALLOP || midcode[i].opr == RETOP)
        {
            int j = i + 1;
            while (j < codeCnt && (midcode[j].opr == SPACEOP || midcode[j].opr == ENDOP)) j++;
            entry[j] = true;
        }
    }

    //开始基本块划分
    //设定第0个基本块为起始块，默认包含全局变量信息，它的后继直接指向main函数
    //若没有全局变量，需手动设置一个空基本块
    if (midcode[0].opr == FUNCOP)
    {
        basicBlock bb;
        bb.id = 0;
        bb.midcodes.clear();
        bb.next.clear();
        bb.prev.clear();
        basicblocks.push_back(bb);
    }

    int i = 0;
    while (i < codeCnt)
    {
        if (entry[i])
        {
            basicBlock bb;
            bb.id = basicblocks.size();
            bb.label = "$NULL";
            bb.midcodes.clear();
            bb.next.clear();
            bb.prev.clear();

            int j = i;
            do
            {
                if (midcode[j].opr != SPACEOP)
                {
                    QCODE tmid;
                    tmid.opr = midcode[j].opr;
                    tmid.lvar = midcode[j].lvar;
                    tmid.rvar = midcode[j].rvar;
                    tmid.ret = midcode[j].ret;

                    bb.midcodes.push_back(tmid);

                    if (midcode[j].opr == FUNCOP || midcode[j].opr == LABOP)
                    {
                        if (bb.label != "$NULL") cout << "Warning: multiple label in a basic block." << endl;
                        bb.label = midcode[j].ret;
                    }
                }
                j++;
            } while ((j < codeCnt) && (!entry[j]));
            i = j;

            basicblocks.push_back(bb);
        }
        else i++;
    }

    basicblocks[0].label = "$START";
}

void linktoBlock(int src, int dst)
{
    if (src < basicblocks.size() && dst < basicblocks.size())
    {
        basicblocks[src].next.push_back(dst);
        basicblocks[dst].prev.push_back(src);
    }
}

void linktoBlock(int src, string dst)       //连接到指定基本块并建立反向连接
{
    for (int i = 0; i < basicblocks.size(); ++i)
        if (basicblocks[i].label == dst)
            linktoBlock(src, i);
}

void basicBlockLink()
{
    //起始块连接到main函数
    linktoBlock(0, "main");

    //对每个基本块，只需看最后一条四元式(?)
    //jmp：只连接到对应的label
    //jne：连接到对应的label和下一个基本块
    //call：连接到对应的label(函数名)，并把该函数后的所有含ret的基本块连接回去，直到第一个endop
    //ret/end：不连
    //其他：连接到下一个基本块
    for (int i = 1; i < basicblocks.size(); ++i)
    {
        int last = basicblocks[i].midcodes.size();
        if (last > 0)
            switch (basicblocks[i].midcodes[last-1].opr)
            {
                case JMPOP: linktoBlock(i, basicblocks[i].midcodes[last-1].ret);
                            break;

                case JNEOP: linktoBlock(i, basicblocks[i].midcodes[last-1].ret);
                            linktoBlock(i, i+1);

                case CALLOP:
                            break;

                case RETOP:
                case ENDOP: break;

                default:    linktoBlock(i, i+1);
            }
    }
}

void basicBlockPrintf(string filename)
{
    ofstream outfile;
    outfile.open(filename, ios::out);
    for (int i = 0; i < basicblocks.size(); ++i)
    {
        outfile << "Basic Block " << i << ": Label = " << basicblocks[i].label << endl;
        for (int j = 0; j < basicblocks[i].midcodes.size(); ++j)
            outfile << oprstr[(int)basicblocks[i].midcodes[j].opr] << ", "
                    << basicblocks[i].midcodes[j].lvar << ", "
                    << basicblocks[i].midcodes[j].rvar << ", "
                    << basicblocks[i].midcodes[j].ret << endl;

        outfile << endl;
    }
    outfile.close();
}
