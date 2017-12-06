#include "stdafx.h"

using namespace std;

symtype symbol_to_symtype(symbolSet x)
{
    switch(x)
    {
        case(VOIDTK): return VOIDTP;
        case(INTTK): return INTTP;
        case(CHARTK): return CHARTP;
        default: return VOIDTP;
    }
}

char * symkind_to_str(symkind x)
{
    switch(x)
    {
        SYMBOL_CASE(VARKD);
        SYMBOL_CASE(CONSTKD);
        SYMBOL_CASE(FUNCKD);
        SYMBOL_CASE(PARAKD);
        default: return "???";
    }
}

char * symtype_to_str(symtype x)
{
    switch(x)
    {
        SYMBOL_CASE(INTTP);
        SYMBOL_CASE(CHARTP);
        SYMBOL_CASE(VOIDTP);
        default: return "???";
    }
}

void initTable()    //初始化符号表
{
    symbolTable.curpnt = 0; //位置指针初始化，总是指向栈顶空的符号表项
    symbolTable.totSub = 1;   //分程序总数清零
    symbolTable.subpnt[0] = 0;  //第0个分程序为全局程序
}

int insertTable(string name, symkind kind, symtype type, int value, int address, int len)
{
    string nname = name;
	transform(name.begin(), name.end(), nname.begin(), ::tolower);
    if (symbolTable.curpnt >= TABSIZE)
    {
        errmain();
        return -1;
    }

    if (kind == FUNCKD)     //函数类符号，需要检查所有分程序，是否有重复定义的函数名
    {
        for (int i = 1; i <= symbolTable.totSub; i++)
            if (symbolTable.item[symbolTable.subpnt[i]].name == nname)
            {   //分程序索引表指向的那个位置总是函数，只需检查对应的标识符是否重复
                errmain();
                return -1;
            }
        //没有重复，将分程序数+1，记录对应位置的索引
        symbolTable.subpnt[++symbolTable.totSub] = symbolTable.curpnt;
    }
    else
    {   
        //其他类型的符号(变量，常量，参数)只需从当前层的起始位置开始找
        for (int i = symbolTable.subpnt[symbolTable.totSub]+1; i < symbolTable.curpnt; i++)
            if (symbolTable.item[i].name == nname)
            {
                errmain();
				cout << "Mutiple defined item " << name << endl;
                return -1;
            }
    }

    symbolTable.item[symbolTable.curpnt].name = nname;
    symbolTable.item[symbolTable.curpnt].kind = kind;
    symbolTable.item[symbolTable.curpnt].type = type;
    symbolTable.item[symbolTable.curpnt].value = value;
    symbolTable.item[symbolTable.curpnt].address = address;
    symbolTable.item[symbolTable.curpnt].len = len;

    symbolTable.curpnt++;

    return 0;
}

void updatePara(int para)
{
    //函数声明后返填符号表的参数个数信息
    int pnt = symbolTable.subpnt[symbolTable.totSub];
    symbolTable.item[pnt].len = para;
}

int searchTable(string name, bool findfunc)
{
    string nname = name;
	transform(name.begin(), name.end(), nname.begin(), ::tolower);
    if (findfunc)
    {
        //查找函数
        for (int i = 1; i <= symbolTable.totSub; i++)
            if (symbolTable.item[symbolTable.subpnt[i]].name == nname)
            {
                //找到该函数，返回它在符号表中的索引
                return symbolTable.subpnt[i];
            }
        //循环结束时仍未找到
        return -1;
    }
    else
    {
        //查找变量、常量和参数
        //当前的分程序序号为totSub，首先在当前层找
    
        for (int i = symbolTable.subpnt[symbolTable.totSub]+1; i < symbolTable.curpnt; i++)
        {
            if (symbolTable.item[i].kind == FUNCKD) //找到下一层函数名，越界了
                break;
            if (symbolTable.item[i].name == nname)   //成功找到
                return i;
        }

        //未在本层中找到，从全局层找
        for (int i = 0; i < symbolTable.subpnt[1]; i++)
            if (symbolTable.item[i].name == nname)
                return i;
        
        //全局变量仍未找到
        return -1;
    }
}

void printSymTable()
{
    cout << "------------------------------Symbol Table Info------------------------------" << endl;
    cout << "Total item count: " << symbolTable.curpnt << endl;
    cout << "Total sub-program count: " << symbolTable.totSub << endl;
    cout << "name\t\t\t" << "kind\t\t\t" << "type\t\t\t" << "value\t\t\t" << "address\t\t\t" << "len" << endl;
    for (int i = 0; i < symbolTable.curpnt; i++)
        cout << symbolTable.item[i].name.c_str() << "\t\t\t"
             << symkind_to_str(symbolTable.item[i].kind) << "\t\t\t"
             << symtype_to_str(symbolTable.item[i].type) << "\t\t\t"
             << symbolTable.item[i].value << "\t\t\t"
             << symbolTable.item[i].address << "\t\t\t"
             << symbolTable.item[i].len << endl;
    cout << "------------------------------Symbol Table End.------------------------------" << endl;
}
