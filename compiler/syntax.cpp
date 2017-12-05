#include "stdafx.h"

using namespace std;

int address;
int para;
int place;

void serror(string msg = ".")
{
    printf("Error on line %d: %s\n", lineNum, msg.c_str());
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
    syntaxDbg = true;                                               //打印语法成分信息
    deepDbg = true;                                                 //打印更详细的信息
    program();                                                      //程序递归子程序
    printSymTable();                                                //打印符号表信息
}

void program()                                                      //程序递归子程序
{
    address = 0;
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

    if (syntaxDbg)
        printf("Line %d: This is a program.\n", lineNum);
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
    constiden = token;
    nextSym();
    if (symbol != ASSTK) serror();                                  //标识符后接等号
    nextSym();
    if (consttype == INTTK)                                         //整型常量
    {
        constival = numericDef();
		if (deepDbg) printf("CONST INT %s = %d\n", constiden.c_str(), constival);
        insertTable(constiden, CONSTKD, INTTP, constival, address++, 0);
    }
    else
    {
        constcval = token[0];
		if (deepDbg) printf("CONST CHAR %s = '%c'\n", constiden.c_str(), constcval);
        insertTable(constiden, CONSTKD, CHARTP, (int)constcval, address++, 0);
    }
    nextSym();
    while (symbol == COMMATK)
    {
        nextSym();
        if (symbol != IDENTK) serror();                                 //类型后应接标识符
		constiden = token;
        nextSym();
        if (symbol != ASSTK) serror();                                  //标识符后接等号
        nextSym();
        if (consttype == INTTK)                                         //整型常量
        {
            constival = numericDef();
			if (deepDbg) printf("CONST INT %s = %d\n", constiden.c_str(), constival);
            insertTable(constiden, CONSTKD, INTTP, constival, address++, 0);
        }
        else
        {
            constcval = token[0];
			if (deepDbg) printf("CONST CHAR %s = '%c'\n", constiden.c_str(), constcval);
            insertTable(constiden, CONSTKD, CHARTP, (int)constcval, address++, 0);
        }
        nextSym();
    }
    if (symbol != SEMITK) serror();
    nextSym();
    if (syntaxDbg)
        printf("Line %d: This is a constant definition statement.\n", lineNum);
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
    if (syntaxDbg)
        printf("Line %d: This is a variable definition statement.\n", lineNum);
}

void varDef()                                                       //变量定义
{
    //此时已预读到第三个符号，symbol可能是逗号、分号或左中括号
    //variden存放变量名，vartype存放类型
    symtype vartp = symbol_to_symtype(vartype);
    if (symbol == LIPARTK)
    {
        nextSym();
        if (symbol != NDIGV && symbol != UINTV) serror();   //数组长度为无符号整数
        arrlen = num;

		if (deepDbg) printf("VAR %s ARRAY %s LEN = %d\n", symbol_type_to_str(vartype), variden.c_str(), arrlen);
        
        insertTable(variden, VARKD, vartp, 0, address, arrlen);
        address += arrlen;

        nextSym();
        if (symbol != RIPARTK) serror(); //声明后应该为右中括号，否则报错
        nextSym();
    }
    else
    {
        //非数组类型变量
		if (deepDbg) printf("VAR %s %s\n", symbol_type_to_str(vartype), variden.c_str());
        insertTable(variden, VARKD, vartp, 0, address++, 0);
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

			if (deepDbg) printf("VAR %s ARRAY %s LEN = %d\n", symbol_type_to_str(vartype), variden.c_str(), arrlen);

            insertTable(variden, VARKD, vartp, 0, address, arrlen);
            address += arrlen;

            nextSym();
            if (symbol != RIPARTK) serror(); //声明后应该为右中括号，否则报错
            nextSym();
        }
        else
        {
            //非数组类型变量
			if (deepDbg) printf("VAR %s %s\n", symbol_type_to_str(vartype), variden.c_str());
            insertTable(variden, VARKD, vartp, 0, address++, 0);
        }
    }
}

void funcDef()                                                      //函数定义
{
    //此时已向前预读三个，读到左括号或左大括号
    symtype funcret = symbol_to_symtype(functype);
    address = 0;
    para = 0;       //重置地址和参数个数变量

	if (deepDbg) printf("FUNCTION DEFINATION NAME: %s, RETURN TYPE: %s\n", funciden.c_str(), symbol_type_to_str(functype));
    insertTable(funciden, FUNCKD, funcret, 0, address++, para);
    
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
        if (syntaxDbg)
            printf("Line %d: This is a function definition statement.\n", lineNum);
    }
    else serror();  //无左大括号，不符合文法要求
}

void paramList()                                                    //参数列表
{
    if (symbol != INTTK && symbol != CHARTK) serror();  //类型标识符
    vartype = symbol;
    nextSym();
    if (symbol != IDENTK) serror();
    variden = token;
    nextSym();
	if (deepDbg) printf("PARAMETER %s %s\n", symbol_type_to_str(vartype), variden.c_str());
    insertTable(variden, PARAKD, symbol_to_symtype(vartype), 0, address++, 0);
    para++;
    while (symbol == COMMATK)
    {
        nextSym();
        if (symbol != INTTK && symbol != CHARTK) serror();  //类型标识符
        vartype = symbol;
        nextSym();
        if (symbol != IDENTK) serror();
        variden = token;
        nextSym();
		if (deepDbg) printf("PARAMETER %s %s\n", symbol_type_to_str(vartype), variden.c_str());
        insertTable(variden, PARAKD, symbol_to_symtype(vartype), 0, address++, 0);
        para++;
    }
    updatePara(para);
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
    address = 0;
    para = 0;
    if (symbol != LPARTK) serror();
    nextSym();
    if (symbol != RPARTK) serror();
    nextSym();
	if (deepDbg) printf("MAIN FUNCTION\n");
    insertTable(funciden, FUNCKD, VOIDTP, 0, address++, 0);
    if (symbol == LBRACETK) //之后是左大括号
    {
        nextSym();
        compound();
        if (symbol != RBRACETK) serror();
        nextSym();
        if (symbol != EOFTK) serror();  //主函数后，程序结束
        if (syntaxDbg)
            printf("Line %d: This is a main function.\n", lineNum);
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
            if (symbol != RBRACETK) serror();
            nextSym();
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
			if (syntaxDbg) printf("Line %d: This is an empty statement.\n", lineNum);
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
		default: serror();
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
    //nextSym();  //预读下一个单词
    if (syntaxDbg) printf("Line %d: This is a IF statement.\n", lineNum);
}

void condition()
{
    expr();
    if (symbol == LESTK || symbol == LEQTK || symbol == GRETK || symbol == GEQTK || symbol == NEQTK || symbol == EQUTK) //关系运算符
    {
        nextSym();
        expr();
    }
    //nextSym();
}

void whileState()
{
    if (symbol != LPARTK) serror(); //条件前应加括号
    nextSym();
    condition();
    if (symbol != RPARTK) serror();
    nextSym();
    statement();
    //nextSym();
    if (syntaxDbg) printf("Line %d: This is a WHILE statement.\n", lineNum);
}

void funcCall()
{
    //函数调用语句，此时已预读到左括号或分号，leftiden存放函数名
    int funcplace = searchTable(leftiden, true);
    if (funcplace == -1) serror();  //未定义的函数

    para = 0;   //参数数目

    if (symbol == LPARTK)   //左括号后跟值参数表
    {
        nextSym();
        paramVal();
        if (symbol != RPARTK) serror(); //值参数表后接右括号
        nextSym();
    }
    //否则为无参函数调用

    if (para != symbolTable.item[funcplace].len) serror();  //传参个数与函数声明中的参数个数不同，报错
    if (syntaxDbg) printf("Line %d: This is a function call statement.\n", lineNum);
}

void paramVal()
{
    expr(); //至少有一个表达式
    para++;
    while (symbol != RPARTK)
    {
		if (symbol != COMMATK) serror();
        nextSym();
        expr();
        para++;
    }
}

void assignState()
{
    //赋值语句，此时已预读到赋值符号或左中括号，leftiden存标识符

    int varplace = searchTable(leftiden, false);
    if (varplace == -1) serror();   //未定义的标识符
    else if (symbolTable.item[varplace].type == CONSTKD) serror();  //不允许向常数赋值

    if (symbol == LIPARTK)  //左中括号，数组元素赋值
    {
        if (symbolTable.item[varplace].len == 0) serror();  //标识符对应符号不是数组
        nextSym();
        expr();
        if (symbol != RIPARTK) serror();
        nextSym();  //此时symbol应当为赋值符号
    }
    if (symbol != ASSTK) serror();
    nextSym();
    expr();
    if (syntaxDbg) printf("Line %d: This is a assign statement.\n", lineNum);
}

void readState()
{
    if (symbol != LPARTK) serror(); //标识符前应加左括号
    nextSym();
    if (symbol != IDENTK) serror(); //scanf语句至少有一个标识符
    leftiden = token;   //此时leftiden存放标识符

    place = searchTable(leftiden, false);
    if (place == -1) serror();  //未定义的变量
    else if (symbolTable.item[place].kind == CONSTKD) serror(); //向常量赋值，不允许
    else if (symbolTable.item[place].len > 0) serror(); //本文法读语句不支持数组读取

    nextSym();
    while (symbol == COMMATK)
    {
        nextSym();
        if (symbol != IDENTK) serror();
        leftiden = token;   //此时leftiden存放标识符

        place = searchTable(leftiden, false);
        if (place == -1) serror();  //未定义的变量
        else if (symbolTable.item[place].kind == CONSTKD) serror(); //向常量赋值，不允许
        else if (symbolTable.item[place].len > 0) serror(); //本文法读语句不支持数组读取

        nextSym();
    }
    if (symbol != RPARTK) serror();
    nextSym();
    if (syntaxDbg) printf("Line %d: This is a read statement.\n", lineNum);
}

void writeState()
{
    if (symbol != LPARTK) serror(); //printf后应加括号
    nextSym();
    if (symbol != STRINGV)  //情况3：括号内只有表达式
    {
        expr();
        //nextSym();
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
    if (syntaxDbg) printf("Line %d: This is a write statement.\n", lineNum);
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
    if (syntaxDbg) printf("Line %d: This is a SWITCH statement.\n", lineNum);
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
    if (syntaxDbg) printf("Line %d: This is a case-sub statement.\n", lineNum);
}

void caseDefault()
{
    if (symbol != COLONTK) serror();
    nextSym();
    statement();
    if (syntaxDbg) printf("Line %d: This is a case-default statement.\n", lineNum);
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
    if (syntaxDbg) printf("Line %d: This is a return statement.\n", lineNum);
    
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
        //查符号表
        place = searchTable(leftiden, false);
        if (symbol == LIPARTK)
        {
            //数组，只可能是变量
            if (place == -1 || (place != -1 && (symbolTable.item[place].len) == 0)) serror();
            else 
                if (symbolTable.item[place].kind == CONSTKD) serror();
            nextSym();
            expr();
            if (symbol != RIPARTK) serror();
            nextSym();
        }
        else if (symbol == LPARTK)
        {
            //有返回值函数调用(有参)，当前符号：左括号
            //nextSym();
            funcCall();
        }
        else
        {
            //标识符或有返回值函数调用(无参)，当前符号：其他(可能是分号)
            //nextSym();
            //首先检查是否为函数
            if ((place = searchTable(leftiden, true)) != -1)
                funcCall();
            else 
            {   //不是函数，是变量或常量
                place = searchTable(leftiden, false);

                if (place == -1) serror();  //都不是，报错
                else if (symbolTable.item[place].len > 0) serror(); //此处不应出现数组
            }
        }

    }
}
