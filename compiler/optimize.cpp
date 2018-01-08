#include "stdafx.h"

using namespace std;
    
vector<DAGItem> nodelist;
vector<DAGNode> DAG;
DAGNode *DAGroot;
int DAGcnt;

void optimize()
{
    constCombine();
    basicBlockPartition();
    basicBlockLink();
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
        if (midcode[i].opr == JNEOP || midcode[i].opr == JMPOP || midcode[i].opr == RETOP)
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

                case JNEOP: linktoBlock(i, i+1);
                            linktoBlock(i, basicblocks[i].midcodes[last-1].ret);

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
        
        outfile << "---------------------------------------------------------" << endl;
        outfile << "prev block: ";
        for (int j = 0; j < basicblocks[i].prev.size(); ++j)
            outfile << basicblocks[i].prev[j] << " ";
        outfile << endl;
        outfile << "next block: ";
        for (int j = 0; j < basicblocks[i].next.size(); ++j)
            outfile << basicblocks[i].next[j] << " ";
        outfile << endl;
        outfile << "---------------------------------------------------------" << endl;

        for (int j = 0; j < basicblocks[i].midcodes.size(); ++j)
            outfile << oprstr[(int)basicblocks[i].midcodes[j].opr] << ", "
                    << basicblocks[i].midcodes[j].lvar << ", "
                    << basicblocks[i].midcodes[j].rvar << ", "
                    << basicblocks[i].midcodes[j].ret << endl;

        outfile << endl;
    }
    outfile.close();
}

int findorAddItem(string name)
{
    for (int i = 0; i < nodelist.size(); ++i)
        if (nodelist[i].name == name)
            return nodelist[i].id;
    
    DAGItem leaf;
    leaf.id = DAG.size();
    leaf.name = name;
    if (!isInt(name[0])) leaf.init = true;
    else leaf.init = false;
    nodelist.push_back(leaf);

    DAGNode node;
    node.id = DAG.size();
    node.name = name;
    node.parent.clear();
    node.left = NULL;
    node.right = NULL;
    DAG.push_back(node);
    return leaf.id;
}

void findorUpdateItem(string name, int k)
{
    for (int i = 0; i < nodelist.size(); ++i)
        if (nodelist[i].name == name)
        {
            nodelist[i].id = k;
            nodelist[i].init = false;
            return;
        }

    DAGItem leaf;
    leaf.id = k;
    leaf.name = name;
    leaf.init = false;
    nodelist.push_back(leaf);
}

int findorAddNode(string op, int li, int ri)
{
    for (int i = 0; i < DAG.size(); ++i)
        if (DAG[i].name == op && (DAG[i].left != NULL && (DAG[i].left)->id == li) &&
            (ri == -1 || (DAG[i].right != NULL && (DAG[i].right)->id == ri)))
            return DAG[i].id;
    
    DAGNode node;
    node.id = DAG.size();
    node.name = op;
    node.parent.clear();
    node.left = &(DAG[li]);
    node.right = ri == -1 ? NULL : &(DAG[ri]);

    int ret = node.id;
    DAG.push_back(node);

    DAG[li].parent.push_back(&(DAG[ret]));
    if (ri != -1)
        DAG[ri].parent.push_back(&(DAG[ret]));

    return ret;
}

void buildDAG(vector<QCODE> midcodes)
{
    DAGroot = NULL;
    DAG.clear();
    nodelist.clear();
    DAGcnt = 0;

    for (int i = 0; i < midcodes.size(); ++i)
        if (midcodes[i].opr == ADDOP || midcodes[i].opr == SUBOP || midcodes[i].opr == MULOP || midcodes[i].opr == DIVOP ||
            midcodes[i].opr == ASSOP || midcodes[i].opr == ASSAOP || midcodes[i].opr == AASSOP)
            {
                int li = findorAddItem(midcodes[i].lvar);
                int ri = midcodes[i].rvar == " " ? -1 : findorAddItem(midcodes[i].rvar);
                int k = findorAddNode(oprstrr[(int)midcodes[i].opr], li, ri);
                findorUpdateItem(midcodes[i].ret, k);
            }
}

void DAGPrintf(string filename)
{
    ofstream outfile;
    outfile.open(filename, ios::out);
    outfile << "------------DAG info------------" << endl;
    for (int i = 0; i < DAG.size(); ++i)
    {
        outfile << "id = " << DAG[i].id << ", name = " << DAG[i].name << endl;
        outfile << "parent :";
        for (int j = 0; j < DAG[i].parent.size(); ++j)
            outfile << " " << DAG[i].parent[j];
        outfile << endl;
        if (DAG[i].left != NULL)
            outfile << "left : " << (DAG[i].left)->id << endl;
        if (DAG[i].right != NULL)
            outfile << "left : " << (DAG[i].right)->id << endl;
        outfile << endl;
    }
    outfile << "------------DAG end-------------" << endl;
    outfile << "Nodelist: " << endl;
    outfile << "name\t\tid\t\tinit" << endl;
    for (int i = 0; i < nodelist.size(); ++i)
        outfile << nodelist[i].name << "\t" << nodelist[i].id << "\t" << nodelist[i].init << endl;
    outfile << "nodelist end" << endl;
    outfile.close();
}
