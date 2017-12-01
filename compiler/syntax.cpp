#include "stdafx.h"

using namespace std;

string lastToken, variden, funciden, leftiden; //上一个标识符
bool syntaxDbg;
extern char nowchar;
symbolSet vartype, functype;
int arrlen;
symbolSet consttype;
int constival;
char constcval;

void serror()
{
    cout << "Error!" << endl;
}

void nextSym()
{
    char ch;                                                        //当前符号
    if ((ch = getchar()) != EOF)
    {
        ungetc(ch, stdin);
        getSym();
    }
    else
    {
        symbol = EOFTK;
    }
}

void syntax()
{
    cnt = 0;                                                        //单词计数
    lineNum = 1;                                                    //行号计数
    nextSym();                                                      //预读一个单词
    syntaxDbg = true;
    program();                                                      //程序递归子程序
}

void program()                                                      //程序递归子程序
{
    if (symbol == CONSTTK)                                          //常量标识，调用常量声明子程序
    {
        nextSym();
        constDec();                                                 //常量声明子程序
    }

    while (symbol == INTTK || symbol == CHARTK)                     //变量声明或有返回值函数定义
    {
        // int a, b; int b; int a[3];
        // int a(int x, int y){} int a{}
        vartype = symbol;                                           //记录当前的变量类型(int or char)
        nextSym();                                                  //预读标识符
        if (symbol != IDENTK) serror();                             //不是标识符，则报错
        variden = token;                                          //保存标识符
        nextSym();                                                  //再预读一个
        if (symbol == COMMATK || symbol == SEMITK || symbol == LIPARTK)//逗号或分号或左中括号，变量声明
        {
            varDec();
        }
        else if (symbol == LPARTK || symbol == LBRACETK)            //左括号或左大括号，有返回值函数定义
        {
            funciden = variden;
            functype = vartype;
            funcDef();                                       //此时已预读三个，读到左括号或左大括号
        }
        else serror();
    }

    while (symbol == VOIDTK || symbol == INTTK || symbol == CHARTK) //若干个无返回值或有返回值函数定义
    {
        // int a(int x, int y){} int a{}
        functype = symbol;                                //函数类型
        nextSym();
        funciden = token;                                          //函数名
        nextSym();
        if (funciden == "main")                                        //主函数
        {
            if (functype != VOIDTK) serror();   //根据文法要求，主函数类型必为void
            mainFunc();
        }
        else
        {
            funcDef();
        }
    }
}

void constDec()                                                     //常量声明
{
    constDef();
    while (symbol == CONSTTK)
    {
        nextSym();                                                  //预读一个单词
        constDef();                                                 //常量定义
    }
}

void constDef()                                                     //常量定义
{
    // int x = 3, y = +69; int x = -3; char x = 'v';
    consttype = symbol;
    if (symbol != INTTK && symbol != CHARTK) serror();              //常量类型错误
    nextSym();
    if (symbol != IDENTK) serror();                                 //类型后应接标识符
    nextSym();
    if (symbol != ASSTK) serror();                                  //标识符后接等号
    nextSym();
    if (consttype == INTTK)                                         //整型常量
    {
        constival = numericDef();
    }
    else
    {
        constcval = token[0];
    }
    nextSym();
    while (symbol == COMMATK)
    {
        nextSym();
        if (symbol != IDENTK) serror();                                 //类型后应接标识符
        nextSym();
        if (symbol != ASSTK) serror();                                  //标识符后接等号
        nextSym();
        if (consttype == INTTK)                                         //整型常量
        {
            constival = numericDef();
        }
        else
        {
            constcval = token[0];
        }
        nextSym();
    }
    if (symbol != SEMITK) serror();
    nextSym();
}

int numericDef()                                                    //读取整数
{
    int sign = 1, val = 0;                                          //符号
    if (symbol == PLUSTK)
        nextSym();
    if (symbol == MINUSTK)
    {
        sign = -1;
        nextSym();
    }
    if (symbol != UINTV && symbol != DIGV && symbol != NDIGV)
        serror();
    val = num;
    return val * sign;
}

void varDec()                                                       //变量声明
{
    varDef();
    if (symbol != SEMITK) serror();
    nextSym();
}

void varDef()                                                       //变量定义
{
    //此时已预读到第三个符号，symbol可能是逗号、分号或左中括号
    //variden存放变量名，vartype存放类型
    if (symbol == LIPARTK)
    {
        nextSym();
        if (symbol != NDIGV && symbol != UINTV) serror();   //数组长度为无符号整数
        arrlen = num;
        nextSym();
        if (symbol != RIPARTK) serror(); //声明后应该为右中括号，否则报错
        nextSym();
    }
    while (symbol == COMMATK) //逗号，说明还有变量声明
    {
        nextSym();
        if (symbol != IDENTK) serror();
        variden = token;
        nextSym();
        if (symbol == LIPARTK)
        {
            nextSym();
            if (symbol != NDIGV && symbol != UINTV) serror();   //数组长度为无符号整数
            arrlen = num;
            nextSym();
            if (symbol != RIPARTK) serror(); //声明后应该为右中括号，否则报错
            nextSym();
        }
    }
}

void funcDef()                                                      //函数定义
{
    //此时已向前预读三个，读到左括号或左大括号
    if (symbol == LPARTK)   //左括号，有参数
    {
        nextSym();          //向前预读一个
        paramList();
        if (symbol != RPARTK) serror(); //参数列表应以小括号结尾
        nextSym();
    }
    if (symbol == LBRACETK) //左大括号，复合语句
    {
        nextSym();
        compound();
        if (symbol != RBRACETK) serror();   //复合语句应以右大括号结尾
        nextSym();
    }
}

void paramList()                                                    //参数列表
{
    if (symbol != INTTK && symbol != CHARTK) serror();  //类型标识符
    vartype = symbol;
    nextSym();
    if (symbol != IDENTK) serror();
    variden = token;
    nextSym();
    while (symbol == COMMATK)
    {
        nextSym();
        if (symbol != INTTK && symbol != CHARTK) serror();  //类型标识符
        vartype = symbol;
        nextSym();
        if (symbol != IDENTK) serror();
        variden = token;
        nextSym();
    }
}

void compound()                                                     //复合语句
{
    if (symbol == CONSTTK)  //常量声明
    {
        nextSym();
        constDec();
    }

    while (symbol == INTTK || symbol == CHARTK)                     //变量声明
    {
        // int a, b; int b; int a[3];
        vartype = symbol;                                           //记录当前的变量类型(int or char)
        nextSym();                                                  //预读标识符
        if (symbol != IDENTK) serror();                             //不是标识符，则报错
        variden = token;                                          //保存标识符
        nextSym();                                                  //再预读一个
        if (symbol == COMMATK || symbol == SEMITK || symbol == LIPARTK)//逗号或分号或左中括号，变量声明
        {
            varDec();
        }
        else
            serror();
    }

    statementList();    //语句列
}

void mainFunc()                                                     //主函数
{
    //此时已向前预读三个，读到左括号，根据文法要求，主函数main标识符后必有一对空的小括号
    if (symbol != LPARTK) serror();
    nextSym();
    if (symbol != RPARTK) serror();
    nextSym();
    if (symbol == LBRACETK) //之后是左大括号
    {
        nextSym();
        compound();
        if (symbol != RBRACETK) serror();
        nextSym();
        if (symbol != EOFTK) serror();  //主函数后，程序结束
    }
    else serror();
}

void statementList()                                                //语句列
{
    while (symbol != RBRACETK)  //遇到右大括号，说明函数体结束了，返回
    {
        //否则就是语句列内的内容
        statement();
    }
}

void statement()                                                    //语句
{
    switch (symbol)
    {
        case(IFTK): 
        {
            nextSym();  //预读一个字符
            ifState();
            break;
        }
        case(WHILETK):
        {
            nextSym();
            whileState();
            break;
        }
        case(LBRACETK):
        {
            nextSym();
            statementList();
            break;
        }
        case(IDENTK):   //标识符，可能是函数调用或赋值语句
        {
            leftiden = token;
            
            break;
        }
        case(SCANFTK):
        {
            nextSym();
            readState();
            break;
        }
        case(PRINTFTK):
        {
            nextSym();
            writeState();
            break;
        }
        case(SEMITK):
        {
            //空语句
            nextSym();
            break;
        }
        case(CASETK):
        {
            nextSym();
            caseState();
            break;
        }
        case(RETURNTK):
        {
            nextSym();
            returnState();
            break;
        }
    }
}
