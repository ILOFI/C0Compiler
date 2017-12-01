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
    else if (symbol == MINUSTK)
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
            leftiden = token;   //leftiden存放待赋值变量名或函数名
            nextSym();
            if (symbol == ASSTK || symbol == LIPARTK)   //赋值符号或左中括号（数组元素），赋值语句
                assignState();
            else if (symbol == LPARTK || symbol == SEMITK)  //左括号或分号（无参数），函数调用语句
                funcCall();
            else serror();
            if (symbol != SEMITK)   //简单语句应以分号结尾
                serror();
            nextSym();  //预读下一个字符
            break;
        }
        case(SCANFTK):
        {
            nextSym();
            readState();
            if (symbol != SEMITK) serror();
            nextSym();
            break;
        }
        case(PRINTFTK):
        {
            nextSym();
            writeState();
            if (symbol != SEMITK) serror();
            nextSym();
            break;
        }
        case(SEMITK):
        {
            //空语句
            nextSym();
            break;
        }
        case(SWITCHTK):
        {
            nextSym();
            switchState();
            break;
        }
        case(RETURNTK):
        {
            nextSym();
            returnState();
            if (symbol != SEMITK) serror();
            nextSym();
            break;
        }
    }
}

void ifState()
{
    if (symbol != LPARTK) serror(); //条件前应加括号
    nextSym();
    condition();
    if (symbol != RPARTK) serror();
    nextSym();
    statement();
    if (symbol == ELSETK)
    {
        nextSym();
        statement();
    }
    nextSym();  //预读下一个单词
}

void condition()
{
    expr();
    if (symbol == LESTK || symbol == LEQTK || symbol == GRETK || symbol == GEQTK || symbol == NEQTK || symbol == EQUTK) //关系运算符
    {
        nextSym();
        expr();
    }
    nextSym();
}

void whileState()
{
    if (symbol != LPARTK) serror(); //条件前应加括号
    nextSym();
    condition();
    if (symbol != RPARTK) serror();
    nextSym();
    statement();
    nextSym();
}

void funcCall()
{
    //函数调用语句，此时已预读到左括号或分号，leftiden存放函数名
    if (symbol == LPARTK)   //左括号后跟值参数表
    {
        nextSym();
        paramVal();
        if (symbol != RPARTK) serror(); //值参数表后接右括号
        nextSym();
    }
    //否则为无参函数调用
}

void paramVal()
{
    expr(); //至少有一个表达式
    while (symbol != RPARTK)
    {
        expr();
        nextSym();
    }
}

void assignState()
{
    //赋值语句，此时已预读到赋值符号或左中括号，leftiden存标识符
    if (symbol == LIPARTK)  //左中括号，数组元素赋值
    {
        nextSym();
        expr();
        if (symbol != RIPARTK) serror();
        nextSym();  //此时symbol应当为赋值符号
    }
    if (symbol != ASSTK) serror();
    nextSym();
    expr();
}

void readState()
{
    if (symbol != LPARTK) serror(); //标识符前应加左括号
    nextSym();
    if (symbol != IDENTK) serror(); //scanf语句至少有一个标识符
    nextSym();
    while (symbol == COMMATK)
    {
        nextSym();
        if (symbol != IDENTK) serror();
        nextSym();
    }
    if (symbol != RPARTK) serror();
    nextSym();
}

void writeState()
{
    if (symbol != LPARTK) serror(); //printf后应加括号
    nextSym();
    if (symbol != STRINGV)  //情况3：括号内只有表达式
    {
        expr();
        nextSym();
    }
    else
    {
        //情况1，2：字符串，表达式 / 字符串
        nextSym();
        if (symbol == COMMATK)
        {
            nextSym();
            expr();
        }
    }
    if (symbol != RPARTK) serror();
    nextSym();    
}

void switchState()
{
    if (symbol != LPARTK) serror();
    nextSym();
    expr();
    if (symbol != RPARTK) serror();
    nextSym();
    if (symbol != LBRACETK) serror();
    nextSym();
    if (symbol != CASETK) serror(); //情况表至少会有一个情况子语句
    nextSym();
    caseList(); //情况表
    if (symbol == DEFAULTTK)
    {
        nextSym();
        caseDefault();
    }
    if (symbol != RBRACETK) serror();
    nextSym();
}

void caseList()
{
    caseSubState();
    while (symbol == CASETK)
    {
        nextSym();
        caseSubState();
    }
}

void caseSubState()
{
    if (symbol == CHARV)
        constcval = token[0];
    else
        constival = numericDef();
    nextSym();
    if (symbol != COLONTK) serror();    //常量后应接括号
    nextSym();
    statement();
}

void caseDefault()
{
    if (symbol != COLONTK) serror();
    nextSym();
    statement();
}

void returnState()
{
    if (symbol == LPARTK)   //return后接表达式
    {
        nextSym();
        expr();
        if (symbol != RPARTK) serror();
        nextSym();
    }
    
}

void expr()
{
    if (symbol == PLUSTK || symbol == MINUSTK)
    {
        nextSym();
    }
    term();
    while (symbol == PLUSTK || symbol == MINUSTK)
    {
        nextSym();
        term();
    }
}

void term()
{
    factor();
    while (symbol == STARTK || symbol == DIVTK)
    {
        nextSym();
        factor();
    }
}

void factor()
{
    if (symbol == LPARTK)
    {
        nextSym();
        expr();
        if (symbol != RPARTK) serror();
        nextSym();
    }
    else if (symbol == CHARV)
    {
        constcval = token[0];
        nextSym();
    }
    else if (symbol == PLUSTK || symbol == MINUSTK || symbol == NDIGV || symbol == DIGV || symbol == UINTV)
    {
        constival = numericDef();
        nextSym();
    }
    else
    {
        //标识符，标识符[表达式]，有返回值的函数调用(有参，无参)
        if (symbol != IDENTK) serror();
        leftiden = token;
        nextSym();
        if (symbol == LIPARTK)
        {
            nextSym();
            expr();
            if (symbol != RIPARTK) serror();
            nextSym();
        }
        else if (symbol == LPARTK)
        {
            nextSym();
            funcCall();
        }
        else
        {
            //标识符或有返回值函数调用(无参)
            
            nextSym();
        }

    }
}
