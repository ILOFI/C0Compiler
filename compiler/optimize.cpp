#include "stdafx.h"

using namespace std;
    
vector<DAGItem> nodelist;
vector<DAGNode> DAG;
DAGNode *DAGroot;
int DAGcnt;

set<int> remain;            //还未加入队列的中间结点

ofstream outfile;

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
    node.left = -1;
    node.right = -1;
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
        if (DAG[i].name == op && (DAG[i].left != -1 && DAG[DAG[i].left].id == li) &&
            (ri == -1 || (DAG[i].right != -1 && DAG[DAG[i].right].id == ri)))
            return DAG[i].id;
    
    DAGNode node;
    node.id = DAG.size();
    node.name = op;
    node.parent.clear();
    node.left = li;
    node.right = ri;

    int ret = node.id;
    DAG.push_back(node);

    DAG[li].parent.push_back(ret);
    if (ri != -1)
        DAG[ri].parent.push_back(ret);

    return ret;
}

void buildDAG(vector<QCODE> midcodes)
{
    DAGroot = NULL;
    DAG.clear();
    nodelist.clear();
    DAGcnt = 0;

    for (int i = 0; i < midcodes.size(); ++i)
        if (midcodes[i].opr == ADDOP || midcodes[i].opr == SUBOP || midcodes[i].opr == MULOP || midcodes[i].opr == DIVOP || midcodes[i].opr == AASSOP)
        {
            int li = findorAddItem(midcodes[i].lvar);
            int ri = (midcodes[i].rvar == " ") ? -1 : findorAddItem(midcodes[i].rvar);
            int k = findorAddNode(oprstr[(int)midcodes[i].opr], li, ri);
            findorUpdateItem(midcodes[i].ret, k);
        }
        else if (midcodes[i].opr == ASSOP)
        {
            int li = findorAddItem(midcodes[i].lvar);
            findorUpdateItem(midcodes[i].ret, li);
        }
}

void DAGPrintf()
{
    outfile << "------------DAG info------------" << endl;
    for (int i = 0; i < DAG.size(); ++i)
    {
        outfile << "id = " << DAG[i].id << ", name = " << DAG[i].name << endl;
        outfile << "parent :";
        for (int j = 0; j < DAG[i].parent.size(); ++j)
            outfile << " " << DAG[DAG[i].parent[j]].id;
        outfile << endl;
        if (DAG[i].left != -1)
            outfile << "left : " << DAG[DAG[i].left].id << endl;
        if (DAG[i].right != -1)
            outfile << "right : " << DAG[DAG[i].right].id << endl;
        outfile << endl;
    }
    outfile << "------------DAG end-------------" << endl;
    outfile << "Nodelist: " << endl;
    outfile << "name\tid\tinit" << endl;
    for (int i = 0; i < nodelist.size(); ++i)
        outfile << nodelist[i].name << "\t" << nodelist[i].id << "\t" << nodelist[i].init << endl;
    outfile << "nodelist end" << endl;
    outfile << endl;
}

bool DAGSatisfy(int choose)
{
    for (int i = 0; i < DAG[choose].parent.size(); ++i)
        if (remain.find((int)(DAG[choose].parent[i])) != remain.end())
            return false;
    
    return true;
}

vector<QCODE> DAGExport()
{
    //从DAG图中导出代码
    vector<QCODE> ret;
    ret.clear();

    vector<int> queue;          //放置中间结点的队列
    queue.clear();
    remain.clear();

    for (int i = 0; i < nodelist.size(); ++i)
        if (!isInt(nodelist[i].name[0]))
            remain.insert((int)(nodelist[i].id));

    while (remain.size() > 0)
    {
        //还有中间结点未进入队列
        int choose;
        for (set<int>::reverse_iterator iter = remain.rbegin(); iter != remain.rend(); iter++)
        {
            choose = *iter;
            if (DAGSatisfy(choose)) break;
        }
        
        do
        {
            remain.erase(choose);
            queue.push_back(choose);
            choose = DAG[choose].left;
        } while (choose != -1 && DAGSatisfy(choose));
    }

    cout << "Node Compute path: ";
    for (vector<int>::reverse_iterator iter = queue.rbegin(); iter != queue.rend(); iter++)
        cout << *iter << " ";

    cout << endl;

    return ret;
}

void DAGOptimize(string filename)
{
    outfile.open(filename, ios::out);

    //遍历每个基本块，找到待优化的中间代码，生成DAG图，
    //之后从DAG图中导出对应代码，插入至原来的位置
    for (int i = 0; i < basicblocks.size(); ++i)
    {
        //如何找到需要优化的中间代码?
        //有连续的(四条及以上)表达式计算
        if (basicblocks[i].midcodes.size() < 4) continue;
        vector<QCODE>::iterator istart, iend;
        vector<QCODE> optcodes;
        int now = 0;
        for (vector<QCODE>::iterator iter = basicblocks[i].midcodes.begin(); iter != basicblocks[i].midcodes.end(); iter++)
            if (isExprOp(iter->opr))
            {
                if (now == 0)
                {
                    istart = iter;
                    optcodes.clear();
                }
                optcodes.push_back(*iter);
                now++;
            }
            else
            {
                if (now >= 4)
                {
                    iend = iter;
                    buildDAG(optcodes);
                    DAGPrintf();
                    //todo: 删除已有代码，从DAG中导出代码，插入到istart位置

                    vector<QCODE> after = DAGExport();
                }
                now = 0;
            }
            
        if (now >= 4)
            {
                iend = basicblocks[i].midcodes.end();
                buildDAG(optcodes);
                DAGPrintf();
                //todo: 删除已有代码，从DAG中导出代码，插入到istart位置

                vector<QCODE> after = DAGExport();

            }
    }

    outfile.close();
}
