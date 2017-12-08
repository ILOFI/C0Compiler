#include "stdafx.h"

using namespace std;

int codepnt;    //当前处理到的四元式指针
vector<Localvar> localvar;      //当前子程序的局部变量

void genMipsCode()
{
    initMips();     //初始化，填入数据段信息

    for (; codepnt < codeCnt; codepnt++)
    {
        //输出当前处理的四元式供参考
        mipsfile << "# Source: " << oprstr[(int)(midcode[codepnt].opr)] << ", " << midcode[codepnt].lvar 
                 << ", " << midcode[codepnt].rvar << ", " << midcode[codepnt].ret << endl;
        
        switch (midcode[codepnt].opr)
        {
            //根据不同的运算符生成对应mips指令
            case(LABOP):    lab_code();
                            break;

            case(JMPOP):    jmp_code();
                            break;

            case(HALTOP):   halt_code();
                            break;
            

            
        }
    }
}

void initMips()
{
    char tstr[32];
    localvar.clear();
    codepnt = 0;    //四元式指针初始化

    //生成数据段信息(.data)
    mipsfile << "\t\t.data" << endl;

    //生成全局常量信息
    while (midcode[codepnt].opr == CONSTOP)
    {
        mipsfile << midcode[codepnt].ret << ":\t.word\t" << midcode[codepnt].rvar << endl;
        codepnt++;
    }

    //生成全局变量信息
    while (midcode[codepnt].opr == VAROP)
    {
        if (isInt(midcode[codepnt].rvar[0]))   //数组
        {
            int arlen = atoi(midcode[codepnt].rvar.c_str());
            mipsfile << midcode[codepnt].ret << ":\t.space\t" << arlen*4 << endl;
        }
        else
            mipsfile << midcode[codepnt].ret << ":\t.space\t4" << endl;
        codepnt++;
    }

    //生成常量字符串信息
    for (int i = 0; i < constStrings.size; i++)
    {
        tstr[0] = '\0';
        sprintf(tstr, "_string_%d", i);
        //_string_%d:   .asciiz "字符串内容"
        mipsfile << tstr << ":\t.asciiz\t\"" << constStrings[i] << "\"" << endl; 
    }

    //准备生成代码段信息
    mipsfile << "\t\t.text" << endl;
}

void lab_code()
{
    mipsfile << midcode[codepnt].ret << ":" << endl;
}

void jmp_code()
{
    mipsfile << "\t\tj\t" << midcode[codepnt].ret << endl;
}

void halt_code()
{
    mipsfile << "\t\tli\t$t0\t10" << endl;
    mipsfile << "\t\tsyscall" << endl;
}
