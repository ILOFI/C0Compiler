#include "stdafx.h"

using namespace std;

int codepnt;    //当前处理到的四元式指针
int offset;     //当前局部变量地址偏移
int paravcnt;   //传入的参数个数
vector<Localvar> localvar;      //当前子程序的局部变量

int addLocalVar(string name, int size) //添加局部变量，size表示数组长度，并返回其偏移地址
{
    Localvar lv;
    lv.name = name;
    lv.address = offset;
    localvar.push_back(lv);

    offset += size * 4;
    return lv.address;
}

int findLocalvar(string name)  //查找局部变量，若找到返回其偏移地址，否则返回-1
{
    for (int i = 0; i < localvar.size(); i++)
        if (localvar[i].name == name)
            return localvar[i].address;
    return -1;
}

void genMipsCode()
{
    initMips();     //初始化，填入数据段信息

    for (; codepnt < codeCnt; codepnt++)
    {
        //输出当前处理的四元式供参考
        if (midcode[codepnt].opr != JNEOP)
            mipsfile << "# Source: " << oprstr[(int)(midcode[codepnt].opr)] << ", " << midcode[codepnt].lvar 
                    << ", " << midcode[codepnt].rvar << ", " << midcode[codepnt].ret << endl;
            
        switch (midcode[codepnt].opr)
        {
            //根据不同的运算符生成对应mips指令
            case(VAROP):    var_code();
                            break;
            
            case(ADDOP):    
            case(SUBOP):                
            case(MULOP):                
            case(DIVOP):    calc_code();
                            break;
                        
            case(FUNCOP):   func_code();
                            break;

            case(SCNFOP):   scnf_code();
                            break;

            case(PRNTOP):   prnt_code();
                            break;

            case(RETOP):    ret_code();
                            break;

            case(LABOP):    lab_code();
                            break;

            case(PARAOP):   para_code();
                            break;

            case(PARAVOP):  parav_code();
                            break;
                        
            case(CALLOP):   call_code();
                            break;

            case(JNEOP):    jne_code();
                            break;

            case(JMPOP):    jmp_code();
                            break;

            case(GREOP):
            case(LESOP):
            case(EQUOP):
            case(NEQOP):
            case(GEQOP):
            case(LEQOP):    relate_code();
                            break;

            case(ASSOP):    ass_code();
                            break;

            case(ASSAOP):   assa_code();
                            break;

            case(AASSOP):   aass_code();
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
    paravcnt = 0;

    //生成数据段信息(.data)
    mipsfile << "\t.data" << endl;

    //生成全局常量信息
    while (midcode[codepnt].opr == CONSTOP)
    {
        mipsfile << midcode[codepnt].ret << ":\t.word\t" << midcode[codepnt].rvar << endl;
        codepnt++;
    }

    //生成全局变量信息
    while (midcode[codepnt].opr == VAROP)
    {
        if (isUint(midcode[codepnt].rvar[0]))   //数组
        {
            int arlen = atoi(midcode[codepnt].rvar.c_str());
            mipsfile << midcode[codepnt].ret << ":\t.space\t" << arlen*4 << endl;
        }
        else
            mipsfile << midcode[codepnt].ret << ":\t.space\t4" << endl;
        codepnt++;
    }

    //生成常量字符串信息
    for (int i = 0; i < constStrings.size(); i++)
    {
        tstr[0] = '\0';
        sprintf(tstr, "._string_%d", i);
        //_string_%d:   .asciiz "字符串内容"
        mipsfile << tstr << ":\t.asciiz\t\"" << constStrings[i] << "\"" << endl; 
    }
    mipsfile << "._string_nl:\t.asciiz\t\"\\n\"" << endl; 

    //准备生成代码段信息
    mipsfile << "\t.text" << endl;
    for (int i = 0; i < 15; ++i)
        mipsfile << "\tmove\t$" << i+11 << "\t$0" << endl;
    mipsfile << "\tj\tmain" << endl;
    mipsfile << "\tnop" << endl;
}

void var_code()
{
    //此处处理函数的局部变量
    if (isUint(midcode[codepnt].rvar[0]))    //数组
    {
        int arlen = atoi(midcode[codepnt].rvar.c_str());
        addLocalVar(midcode[codepnt].ret, arlen);
    }
    else if (!isReg(midcode[codepnt].ret))
        addLocalVar(midcode[codepnt].ret);
}

void calc_code()
{
    int addr;     //三个变量的地址信息
    string op, u, v, w;     //u, v, w表示三个寄存器变量
    switch (midcode[codepnt].opr)
    {
        case(ADDOP): op = "add"; break;
        case(SUBOP): op = "sub"; break;
        case(MULOP): op = "mul"; break;
        case(DIVOP): op = "div"; break;
    }

    //左操作数存t0
    if (isInt(midcode[codepnt].lvar[0]))
    {
        mipsfile << "\tli\t$t0\t" << midcode[codepnt].lvar << endl;
        u = "$t0";
    }
    else if (isReg(midcode[codepnt].lvar))
        u = midcode[codepnt].lvar;
    else
    {
        addr = -1 * findLocalvar(midcode[codepnt].lvar);
        if (addr == 1)     //全局变量，直接加载地址
        {
            mipsfile << "\tla\t$t0\t" << midcode[codepnt].lvar << endl;
            mipsfile << "\tlw\t$t0\t($t0)" << endl;
        }
        else
            mipsfile << "\tlw\t$t0\t" << addr << "($fp)" << endl;
        u = "$t0";
    }

    //右操作数存t1
    if (isInt(midcode[codepnt].rvar[0]))
        v = midcode[codepnt].rvar;
    else if (isReg(midcode[codepnt].rvar))
        v = midcode[codepnt].rvar;
    else
    {
        addr = -1 * findLocalvar(midcode[codepnt].rvar);
        if (addr == 1)
        {
            mipsfile << "\tla\t$t1\t" << midcode[codepnt].rvar << endl;
            mipsfile << "\tlw\t$t1\t($t1)" << endl;
        }
        else
            mipsfile << "\tlw\t$t1\t" << addr << "($fp)" << endl;
        v = "$t1";
    }

    if (isReg(midcode[codepnt].ret)) w = midcode[codepnt].ret;
    else w = "$t0";

    mipsfile << "\t" << op << "\t" << w << "\t" << u << "\t" << v << endl;

    //结果保存
    if (isTempVal(midcode[codepnt].ret))    //结果为四元式的临时变量
    {
        addr = -1 * addLocalVar(midcode[codepnt].ret);
        mipsfile << "\tsw\t" << w << "\t" << addr << "($fp)" << endl;
    }
    else if (!isReg(midcode[codepnt].ret))
    {
        addr = -1 * findLocalvar(midcode[codepnt].ret);
        if (addr == 1)     //全局变量
        {
            mipsfile << "\tla\t$t1\t" << midcode[codepnt].ret << endl;
            mipsfile << "\tsw\t" << w << "\t($t1)" << endl;
        }
        else
            mipsfile << "\tsw\t" << w << "\t" << addr << "($fp)" << endl;
    }
}

void func_code()
{
    lab_code();     //当前函数label
    //进入函数
    //新的活动记录
    //$sp以下位置为新的函数活动记录，0位存prev $fp，-4位存prev $ra，-68位以下存函数参数和局部变量
    mipsfile << "\tmove\t$fp\t$sp" << endl;   //fp为当前sp地址
    mipsfile << "\tsub\t$sp\t$sp\t68" << endl;
    offset = 68;
    localvar.clear();   //清空临时变量表
}

void scnf_code()
{
    //scanf, <type>, , <iden>
    int v0 = midcode[codepnt].lvar == "int" ? 5 : 12;
    int addr = -1 * findLocalvar(midcode[codepnt].ret);   //存储的地址

    mipsfile << "\tli\t$v0\t" << v0 << endl;
    mipsfile << "\tsyscall" << endl;

    if (isReg(midcode[codepnt].ret))
        mipsfile << "\tmove\t" << midcode[codepnt].ret << "\t$v0" << endl;
    else if (addr == 1)
    {
        mipsfile << "\tla\t$t0\t" << midcode[codepnt].ret << endl;
        mipsfile << "\tsw\t$v0\t($t0)" << endl;
    }
    else
        mipsfile << "\tsw\t$v0\t" << addr << "($fp)" << endl;
}

void prnt_code()
{
    //prntf, [<string id>], [<variden or int>], <type>
    if (midcode[codepnt].lvar != " ")   //打印字符串
    {
        mipsfile << "\tli\t$v0\t4" << endl;
        mipsfile << "\tla\t$a0\t" << midcode[codepnt].lvar << endl;
        mipsfile << "\tsyscall" << endl;
    }

    if (midcode[codepnt].rvar != " ")   //打印表达式
    {
        int v0 = midcode[codepnt].ret == "int" ? 1 : 11;
        mipsfile << "\tli\t$v0\t" << v0 << endl;

        if (isInt(midcode[codepnt].rvar[0]))
            mipsfile << "\tli\t$a0\t" << midcode[codepnt].rvar << endl;
        else if (isReg(midcode[codepnt].rvar))
        {
            mipsfile << "\tmove\t$a0\t" << midcode[codepnt].rvar << endl;
        }
        else
        {
            int addr = -1 * findLocalvar(midcode[codepnt].rvar);
            if (addr == 1)
            {
                mipsfile << "\tla\t$a0\t" << midcode[codepnt].rvar << endl;
                mipsfile << "\tlw\t$a0\t($a0)" << endl;
            }
            else
                mipsfile << "\tlw\t$a0\t" << addr << "($fp)" << endl;
        }

        mipsfile << "\tsyscall" << endl;
    }

    newline_code();
}

void ret_code()
{
    //ret, , [<returniden or constint>]
    //约定：$v1存放函数返回值
    if (midcode[codepnt].ret != " ")
    {
        if (isInt(midcode[codepnt].ret[0]))
            mipsfile << "\tli\t$v1\t" << midcode[codepnt].ret << endl;
        else if (isReg(midcode[codepnt].ret))
            mipsfile << "\tmove\t$v1\t" << midcode[codepnt].ret << endl;
        else
        {
            int addr = -1 * findLocalvar(midcode[codepnt].ret);
            if (addr == 1)
            {
                mipsfile << "\tla\t$v1\t" << midcode[codepnt].ret << endl;
                mipsfile << "\tlw\t$v1\t($v1)" << endl;
            }
            else
                mipsfile << "\tlw\t$v1\t" << addr << "($fp)" << endl;
        }
    }
        
    //恢复信息
    mipsfile << "\tmove\t$sp\t$fp" << endl;
    mipsfile << "\tlw\t$fp\t($fp)" << endl;
    mipsfile << "\tmove\t$t0\t$ra" << endl;
    mipsfile << "\tlw\t$ra\t-4($sp)" << endl;
    for (int i = 0; i < 15; ++i)
        mipsfile << "\tlw\t$" << i+11 << "\t" << -8-4*i << "($sp)" << endl;


    mipsfile << "\tjr\t$t0" << endl;
    mipsfile << "\tnop" << endl;
}

void lab_code()
{
    mipsfile << midcode[codepnt].ret << ":" << endl;
}

void para_code()
{
    //para, <type>, , <name>
    //参数的值在调用前已设置好存于栈中，此处只需要注册一下
    //如果是寄存器需要加载出来
    int addr = -1 * addLocalVar(midcode[codepnt].ret);
    if (isReg(midcode[codepnt].ret))
        mipsfile << "\tlw\t" << midcode[codepnt].ret << "\t" << addr << "($fp)" << endl;
}

void parav_code()
{
    //parav, , , <variden or const int>
    string u = "$t0";       //参数值存放位置

    mipsfile << "\tsub\t$sp\t$fp\t" << offset << endl;
    //首先将参数值存入t0中
    if (isInt(midcode[codepnt].ret[0]))
        mipsfile << "\tli\t$t0\t" << midcode[codepnt].ret << endl;
    else if (isReg(midcode[codepnt].ret))
        u = midcode[codepnt].ret;
    else
    {
        int addr = -1 * findLocalvar(midcode[codepnt].ret);
        if (addr == 1)   //全局变量
        {
            mipsfile << "\tla\t$t0\t" << midcode[codepnt].ret << endl;
            mipsfile << "\tlw\t$t0\t($t0)" << endl;
        }
        else
            mipsfile << "\tlw\t$t0\t" << addr << "($fp)" << endl;
    }

    //直接将参数值存入栈的对应位置
    //此时函数call尚未发生，$sp以下位置为新的函数活动记录，0位存prev $fp，-4位存prev $ra，因此函数参数从-68位开始
    mipsfile << "\tsw\t" << u << "\t" << -4*paravcnt-68 << "($sp)" << endl;
    paravcnt++;
}

void call_code()
{
    //call, <funcname>, , [<return variden>]

    mipsfile << "\tsub\t$sp\t$fp\t" << offset << endl;
    //$sp以下位置为新的函数活动记录，0位存prev $fp，-4位存prev $ra，-8位以下存11-25号寄存器，
    //-68位以下存函数参数和局部变量
    mipsfile << "\tsw\t$fp\t($sp)" << endl;
    mipsfile << "\tsw\t$ra\t-4($sp)" << endl;
    for (int i = 0; i < 15; ++i)
        mipsfile << "\tsw\t$" << i+11 << "\t" << -8-4*i << "($sp)" << endl;

    mipsfile << "\tjal\t" << midcode[codepnt].lvar << endl;
    mipsfile << "\tnop" << endl;  //延迟槽

    //约定：$v1存放函数返回值
    if (midcode[codepnt].ret != " ")
    {
        if (isReg(midcode[codepnt].ret))
            mipsfile << "\tmove\t" << midcode[codepnt].ret << "\t$v1" << endl;
        else
        {
            int addr = -1 * addLocalVar(midcode[codepnt].ret);
            mipsfile << "\tsw\t$v1\t" << addr << "($fp)" << endl;
        }
    }

    paravcnt = 0;
}

void jne_code()
{
    //JNE不满足跳转与关系运算符紧邻，只需要输出对应跳转标签即可
    mipsfile << midcode[codepnt].ret << endl;
    mipsfile << "\tnop" << endl;
}

void jmp_code()
{
    mipsfile << "\tj\t" << midcode[codepnt].ret << endl;
    mipsfile << "\tnop" << endl;
}

void relate_code()
{
    //<opr>, <lvar>, <rvar>, 
    //关系运算符后接不满足跳转指令，因此此处生成的跳转指令条件需与原条件相反
    string op, u, v;
    int addr;

    switch (midcode[codepnt].opr)
    {
        case(GREOP): op = "ble"; break; //>  小于等于跳转
        case(LESOP): op = "bge"; break; //<  大于等于跳转
        case(EQUOP): op = "bne"; break; //== 不等于跳转
        case(NEQOP): op = "beq"; break; //!= 等于跳转
        case(GEQOP): op = "blt"; break; //>= 小于跳转
        case(LEQOP): op = "bgt"; break; //<= 大于跳转
    }

    //左操作数
    u = "$t0";
    if (isInt(midcode[codepnt].lvar[0]))
        mipsfile << "\tli\t$t0\t" << midcode[codepnt].lvar << endl;
    else if(isReg(midcode[codepnt].lvar))
        u = midcode[codepnt].lvar;
    else
    {
        addr = -1 * findLocalvar(midcode[codepnt].lvar);
        if (addr == 1)
        {
            mipsfile << "\tla\t$t0\t" << midcode[codepnt].lvar << endl;
            mipsfile << "\tlw\t$t0\t($t0)" << endl;
        }
        else
            mipsfile << "\tlw\t$t0\t" << addr << "($fp)" << endl;
    }

    v = "$t1";
    if (isInt(midcode[codepnt].rvar[0]))
        v = midcode[codepnt].rvar;
    else if (isReg(midcode[codepnt].rvar))
        v = midcode[codepnt].rvar;
    else
    {
        addr = -1 * findLocalvar(midcode[codepnt].rvar);
        if (addr == 1)
        {
            mipsfile << "\tla\t$t1\t" << midcode[codepnt].rvar << endl;
            mipsfile << "\tlw\t$t1\t($t1)" << endl;
        }
        else
            mipsfile << "\tlw\t$t1\t" << addr << "($fp)" << endl;
    }

    mipsfile << "\t" << op << "\t" << u << "\t" << v << "\t";
}

void ass_code()
{
    //=, <variden or const int>, , <dest iden>
    string u = "$t0";
    if (isInt(midcode[codepnt].lvar[0]))
        mipsfile << "\tli\t$t0\t" << midcode[codepnt].lvar << endl;
    else if (isReg(midcode[codepnt].lvar))
        u = midcode[codepnt].lvar;
    else
    {
        int addr = -1 * findLocalvar(midcode[codepnt].lvar);
        if (addr == 1)
        {
            mipsfile << "\tla\t$t0\t" << midcode[codepnt].lvar << endl;
            mipsfile << "\tlw\t$t0\t($t0)" << endl;
        }
        else
            mipsfile << "\tlw\t$t0\t" << addr << "($fp)" << endl;
    }

    if (isReg(midcode[codepnt].ret))
        mipsfile << "\tmove\t" << midcode[codepnt].ret << "\t" << u << endl;
    else
    {
        int addr = -1 * findLocalvar(midcode[codepnt].ret);
        if (addr == 1)
        {
            mipsfile << "\tla\t$t1\t" << midcode[codepnt].ret << endl;
            mipsfile << "\tsw\t" << u << "\t($t1)" << endl;
        }
        else
            mipsfile << "\tsw\t" << u << "\t" << addr << "($fp)" << endl;
    }
}

void assa_code()
{
    // assign array element: []=, result, index, arrname, arrname[index]=result
    //处理result，写入t0
    string u = "$t0";
    if (isInt(midcode[codepnt].lvar[0]))
        mipsfile << "\tli\t$t0\t" << midcode[codepnt].lvar << endl;
    else if (isReg(midcode[codepnt].lvar))
        u = midcode[codepnt].lvar;
    else
    {
        int addr = -1 * findLocalvar(midcode[codepnt].lvar);
        if (addr == 1)
        {
            mipsfile << "\tla\t$t0\t" << midcode[codepnt].lvar << endl;
            mipsfile << "\tlw\t$t0\t($t0)" << endl;
        }
        else
            mipsfile << "\tlw\t$t0\t" << addr << "($fp)" << endl;
    }

    //处理index偏移，写入t1
    string v = "$t1";
    if (isInt(midcode[codepnt].rvar[0]))
        mipsfile << "\tli\t$t1\t" << midcode[codepnt].rvar << endl;
    else if (isReg(midcode[codepnt].rvar))
        v = midcode[codepnt].rvar;
    else
    {
        int addr = -1 * findLocalvar(midcode[codepnt].rvar);
        if (addr == 1)
        {
            mipsfile << "\tla\t$t1\t" << midcode[codepnt].rvar << endl;
            mipsfile << "\tlw\t$t1\t($t1)" << endl;
        }
        else
            mipsfile << "\tlw\t$t1\t" << addr << "($fp)" << endl;
    }

    //处理arrname地址，写入t2
    int addr = -1 * findLocalvar(midcode[codepnt].ret);
    if (addr == 1)  //全局变量，数组地址应该正向延展
    {
        mipsfile << "\tmul\t$t1\t" << v << "\t4" << endl;
        mipsfile << "\tla\t$t2\t" << midcode[codepnt].ret << endl;
    }
    else
    {
        mipsfile << "\tmul\t$t1\t" << v << "\t-4" << endl;
        mipsfile << "\tadd\t$t2\t$fp\t" << addr << endl;
    }

    //计算出数组元素的地址并赋值
    mipsfile << "\tadd\t$t1\t$t1\t$t2" << endl;
    mipsfile << "\tsw\t" << u << "\t($t1)" << endl;
}

void aass_code()
{
    // array element assign: =[], arrname, index, targetiden
    //处理arrname地址，写入t0
    int addr = -1 * findLocalvar(midcode[codepnt].lvar);
    string sign = addr == 1 ? "" : "-";
    if (addr == 1)
        mipsfile << "\tla\t$t0\t" << midcode[codepnt].lvar << endl;
    else
        mipsfile << "\tadd\t$t0\t$fp\t" << addr << endl;

    //处理index偏移，写入t1
    if (isInt(midcode[codepnt].rvar[0]))
        mipsfile << "\tli\t$t1\t" << midcode[codepnt].rvar << endl;
    else if (isReg(midcode[codepnt].rvar))
        mipsfile << "\tmove\t$t1\t" << midcode[codepnt].rvar << endl;
    else
    {
        int addr = -1 * findLocalvar(midcode[codepnt].rvar);
        if (addr == 1)
        {
            mipsfile << "\tla\t$t1\t" << midcode[codepnt].rvar << endl;
            mipsfile << "\tlw\t$t1\t($t1)" << endl;
        }
        else
            mipsfile << "\tlw\t$t1\t" << addr << "($fp)" << endl;
    }
    mipsfile << "\tmul\t$t1\t$t1\t" << sign << "4" << endl;

    //获得数组真实地址并取到值，写入t1
    mipsfile << "\tadd\t$t1\t$t0\t$t1" << endl;
    mipsfile << "\tlw\t$t1\t($t1)" << endl;

    //targetiden为临时变量
    if (isReg(midcode[codepnt].ret))
        mipsfile << "\tmove\t" << midcode[codepnt].ret << "\t$t1" << endl;
    else
    {
        addr = -1 * addLocalVar(midcode[codepnt].ret);
        mipsfile << "\tsw\t$t1\t" << addr << "($fp)" << endl;
    }
}

void halt_code()
{
    mipsfile << "\tli\t$v0\t10" << endl;
    mipsfile << "\tsyscall" << endl;
}

void newline_code()
{
    mipsfile << "\tli\t$v0\t4" << endl;
    mipsfile << "\tla\t$a0\t._string_nl" << endl;
    mipsfile << "\tsyscall" << endl;
}
