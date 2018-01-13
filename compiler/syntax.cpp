#include "stdafx.h"

using namespace std;

int address;
int para;
int place;
symtype exprType;       //表达式的类型(int or char)
bool inMain, isVoidFunc;

void serror(string msg = "Syntax Error")
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
    
    initTable();
    initQuaternion();
    program();                                                      //程序递归子程序
    if (symTableDbg)
        printSymTable();                                            //打印符号表信息
}

void program()                                                      //程序递归子程序
{
    address = 0;
    inMain = false;
    isVoidFunc = false;
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
        if (symbol != IDENTK)
        {
            if (symbol == MAINTK) inMain = true;
            errmain(INVALID_IDENTITY, lineNum, token);                   //非法标识符，则报错
            variden = "$err";                                     //当前变量错误
        }
        else
            variden = token;                                          //保存标识符
        nextSym();                                                  //再预读一个
        if (symbol == COMMATK || symbol == SEMITK || symbol == LIPARTK)//逗号或分号或左中括号，变量声明
        {
            varDec();
        }
        else if (symbol == LPARTK || symbol == LBRACETK)            //左括号或左大括号，有返回值函数定义
        {
            if (inMain) errmain(MAIN_TYPE_ERROR, lineNum);
            inMain = false;
            funciden = variden;
            functype = vartype;
            isVoidFunc = false;
            funcDef();                                       //此时已预读三个，读到左括号或左大括号
        }
        else serror();
    }

    while (symbol == VOIDTK || symbol == INTTK || symbol == CHARTK) //若干个无返回值或有返回值函数定义
    {
        // int a(int x, int y){} int a{}
        isVoidFunc = symbol == VOIDTK;
        functype = symbol;                                //函数类型
        nextSym();
        funciden = token;                                          //函数名

        if (symbol == MAINTK)                                        //主函数
        {
            inMain = true;
            nextSym();
            if (functype != VOIDTK) errmain(MAIN_TYPE_ERROR, lineNum);   //根据文法要求，主函数类型必为void
            mainFunc();
        }
        else
        {
            if (symbol != IDENTK)
            {
                errmain(INVALID_IDENTITY, lineNum, token);
                //token = "$err";
            }
            inMain = false;
            nextSym();
            funcDef();
        }
    }

    if (!inMain) errmain(LACK_OF_MAIN_FUNCTION, lineNum);

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
    if (symbol != INTTK && symbol != CHARTK) 
    {
        errmain(INVALID_TYPE, lineNum, token);              //常量类型错误
        //直接跳到分号
        while (symbol != SEMITK && symbol != EOFTK)
            nextSym();
    }
    else
    /*
    nextSym();
    if (symbol != IDENTK) 
    {
        errmain(INVALID_IDENTITY, lineNum, token);                //类型后应接标识符
        constiden = "$err";
    }
    else
        constiden = token;
    nextSym();
    if (symbol != ASSTK) 
        errmain(CONST_NOT_ASSIGNED, lineNum, constiden);           //标识符后接等号
    nextSym();
    if (consttype == INTTK)                                         //整型常量
    {
        constival = numericDef();
		if (deepDbg) printf("CONST INT %s = %d\n", constiden.c_str(), constival);
        insertTable(constiden, CONSTKD, INTTP, constival, address++, 0);

        genQuaternion(CONSTOP, oprstr[(int)INTOP], constival, constiden);
    }
    else
    {
        constcval = token[0];
		if (deepDbg) printf("CONST CHAR %s = '%c'\n", constiden.c_str(), constcval);
        insertTable(constiden, CONSTKD, CHARTP, (int)constcval, address++, 0);

        genQuaternion(CONSTOP, oprstr[(int)CHAROP], (int)constcval, constiden);
    }
    nextSym();
    */

    do
    {
        nextSym();
        if (symbol != IDENTK) 
        {
            errmain(INVALID_IDENTITY, lineNum, token);                //类型后应接标识符
            //非法标识符，直接跳到下一个逗号或分号
            while (symbol != COMMATK && symbol != SEMITK && symbol != EOFTK)
                nextSym();
            continue;
        }
        else
            constiden = token;
        nextSym();
        if (symbol != ASSTK) 
        {
            errmain(CONST_NOT_ASSIGNED, lineNum, constiden);           //标识符后接等号
            //标识符未赋值（缺少赋值符号），直接跳到下一个逗号或分号
            while (symbol != COMMATK && symbol != SEMITK && symbol != EOFTK)
                nextSym();
            continue;
        }
        nextSym();
        if (consttype == INTTK)                                         //整型常量
        {
            constival = numericDef();
            if (symbol != UINTV && symbol != DIGV && symbol != NDIGV)
            {   //整型常量值错误
                errmain(INVALID_CONST_VALUE, lineNum, token);
                while (symbol != COMMATK && symbol != SEMITK && symbol != EOFTK)
                    nextSym();
                continue;
            }
            if (deepDbg) printf("CONST INT %s = %d\n", constiden.c_str(), constival);
            insertTable(constiden, CONSTKD, INTTP, constival, address++, 0);

            genQuaternion(CONSTOP, oprstr[(int)INTOP], constival, constiden);
        }
        else
        {
            if (symbol != CHARV)
            {   //字符常量值错误
                errmain(INVALID_CONST_VALUE, lineNum, token);
                while (symbol != COMMATK && symbol != SEMITK && symbol != EOFTK)
                    nextSym();
                continue;
            }
            constcval = token[0];
            if (deepDbg) printf("CONST CHAR %s = '%c'\n", constiden.c_str(), constcval);
            insertTable(constiden, CONSTKD, CHARTP, (int)constcval, address++, 0);

            genQuaternion(CONSTOP, oprstr[(int)CHAROP], (int)constcval, constiden);
        }
        nextSym();
    } while (symbol == COMMATK);
    if (symbol != SEMITK)
    {
        errmain(LACK_OF_SEMICOLON, lineNum);
        while (symbol != IDENTK && symbol != CONSTTK && symbol != INTTK && symbol != CHARTK &&
               symbol != VOIDTK && symbol != IFTK && symbol != WHILETK && 
               symbol != SCANFTK && symbol != PRINTFTK && 
               symbol != SWITCHTK && symbol != RETURNTK && symbol != EOFTK)
            nextSym();
    }
    else
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
    val = num;
    return val * sign;
}

void varDec()                                                       //变量声明
{
    varDef();
    if (symbol != SEMITK)
    {
        errmain(LACK_OF_SEMICOLON, lineNum);
    }
    else nextSym();
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
        if (symbol != NDIGV && symbol != UINTV)
        {
            errmain(INVALID_ARRAY_LENGTH, lineNum, token);   //数组长度为无符号整数
            while (symbol != RIPARTK && symbol != COMMATK && symbol != SEMITK && symbol != EOFTK)
                nextSym();
        }
        else if (variden != "$err") //变量名是合法的标识符
        {
            arrlen = num;

            if (deepDbg) printf("VAR %s ARRAY %s LEN = %d\n", symbol_type_to_str(vartype), variden.c_str(), arrlen);
            
            insertTable(variden, VARKD, vartp, 0, address, arrlen);
            address += arrlen;

            genQuaternion(VAROP, oprstr[(int)vartp+2], arrlen, variden);
            nextSym();
        }
        
        if (symbol != RIPARTK) errmain(LACK_OF_RIGHT_BRACKET, lineNum); //声明后应该为右中括号，否则报错
        else nextSym();
    }
    else if (variden != "$err")
    {
        //非数组类型变量
		if (deepDbg) printf("VAR %s %s\n", symbol_type_to_str(vartype), variden.c_str());
        insertTable(variden, VARKD, vartp, 0, address++, 0);

        genQuaternion(VAROP, oprstr[(int)vartp+2], oprstr[(int)SPACEOP], variden);
    }

    while (symbol == COMMATK) //逗号，说明还有变量声明
    {
        nextSym();
        if (symbol != IDENTK)
        {
            errmain(INVALID_IDENTITY, lineNum);                   //非法标识符，则报错
            while (symbol != COMMATK && symbol != SEMITK && symbol != EOFTK)
                nextSym();
            continue;                                     //当前变量错误
        }
        variden = token;
        nextSym();
        if (symbol == LIPARTK)
        {
            nextSym();
            if (symbol != NDIGV && symbol != UINTV)
            {
                errmain(INVALID_ARRAY_LENGTH, lineNum, token);   //数组长度为无符号整数
                while (symbol != COMMATK && symbol != SEMITK && symbol != EOFTK)
                    nextSym();
                continue;  
            }
            arrlen = num;

			if (deepDbg) printf("VAR %s ARRAY %s LEN = %d\n", symbol_type_to_str(vartype), variden.c_str(), arrlen);

            insertTable(variden, VARKD, vartp, 0, address, arrlen);
            address += arrlen;

            genQuaternion(VAROP, oprstr[(int)vartp+2], arrlen, variden);

            nextSym();
            if (symbol != RIPARTK) errmain(LACK_OF_RIGHT_BRACKET, lineNum); //声明后应该为右中括号，否则报错
            else nextSym();
        }
        else
        {
            //非数组类型变量
			if (deepDbg) printf("VAR %s %s\n", symbol_type_to_str(vartype), variden.c_str());
            insertTable(variden, VARKD, vartp, 0, address++, 0);

            genQuaternion(VAROP, oprstr[(int)vartp+2], oprstr[(int)SPACEOP], variden);
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

    genQuaternion(FUNCOP, oprstr[(int)funcret+2], oprstr[(int)SPACEOP], funciden);
    
    if (symbol == LPARTK)   //左括号，有参数
    {
        nextSym();          //向前预读一个
        paramList();
        if (symbol != RPARTK) errmain(LACK_OF_RIGHT_PARENT, lineNum); //参数列表应以小括号结尾
        else nextSym();
    }

    if (symbol != LBRACETK) //无左大括号，报错
        errmain(LACK_OF_LEFT_BRACE, lineNum);
    else nextSym();

    compound();
    if (symbol != RBRACETK) errmain(LACK_OF_RIGHT_BRACE, lineNum);   //复合语句应以右大括号结尾
    else nextSym();
    if (syntaxDbg)
        printf("Line %d: This is a function definition statement.\n", lineNum);

    //注意：此时ret指令默认在return语句中生成，而无返回值函数可能没有return语句?
    //可以在添加endop前检查当前指令是否为return，若不是，手动添加
    checkReturnCode(inMain);
    genQuaternion(ENDOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], oprstr[(int)SPACEOP]);
}

void paramList()                                                    //参数列表
{
    while (symbol != INTTK && symbol != CHARTK && symbol != EOFTK)
    {
        errmain(INVALID_TYPE, lineNum, token);  //类型标识符
        while (symbol != COMMATK && symbol != RPARTK && symbol != LBRACETK && symbol != EOFTK)
            nextSym();
        if (symbol == COMMATK)
            nextSym();
        else
            return;
    }
    vartype = symbol;
    nextSym();
    if (symbol != IDENTK) errmain(INVALID_IDENTITY, lineNum, token);
    variden = token;
    nextSym();
	if (deepDbg) printf("PARAMETER %s %s\n", symbol_type_to_str(vartype), variden.c_str());
    insertTable(variden, PARAKD, symbol_to_symtype(vartype), 0, address++, 0);
    para++;
    
    genQuaternion(PARAOP, oprstr[(int)(symbol_to_symtype(vartype))+2], oprstr[(int)SPACEOP], variden);

    while (symbol == COMMATK)
    {
        nextSym();
        while (symbol != INTTK && symbol != CHARTK && symbol != EOFTK)
        {
            errmain(INVALID_TYPE, lineNum, token);  //类型标识符
            while (symbol != COMMATK && symbol != RPARTK && symbol != LBRACETK && symbol != EOFTK)
                nextSym();
            if (symbol == COMMATK)
                nextSym();
            else
                return;
        }
        vartype = symbol;
        nextSym();
        if (symbol != IDENTK) errmain(INVALID_IDENTITY, lineNum, token);
        variden = token;
        nextSym();
		if (deepDbg) printf("PARAMETER %s %s\n", symbol_type_to_str(vartype), variden.c_str());
        insertTable(variden, PARAKD, symbol_to_symtype(vartype), 0, address++, 0);
        para++;

        genQuaternion(PARAOP, oprstr[(int)(symbol_to_symtype(vartype))+2], oprstr[(int)SPACEOP], variden);
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
        if (symbol != IDENTK)
        {
            errmain(INVALID_IDENTITY, lineNum, token);             //不是标识符，则报错
            variden = "$err";
        }
        else 
            variden = token;                                          //保存标识符
        nextSym();                                                  //再预读一个
        if (symbol == COMMATK || symbol == SEMITK || symbol == LIPARTK)//逗号或分号或左中括号，变量声明
        {
            varDec();
        }
        else
        {
            serror();
            while (symbol != EOFTK && symbol != IDENTK && symbol != INTTK && symbol != CHARTK &&
               symbol != IFTK && symbol != WHILETK && 
               symbol != SCANFTK && symbol != PRINTFTK && 
               symbol != SWITCHTK && symbol != RETURNTK && symbol != LBRACETK && symbol != RBRACETK)
            nextSym();
        }
    }

    statementList();    //语句列
}

void mainFunc()                                                     //主函数
{
    //此时已向前预读三个，读到左括号，根据文法要求，主函数main标识符后必有一对空的小括号
    address = 0;
    para = 0;
    if (symbol != LPARTK) errmain(LACK_OF_LEFT_PARENT, lineNum);
    
    nextSym();
    while (symbol != RPARTK && symbol != LBRACETK && symbol != EOFTK)
        nextSym();
    
    if (symbol != RPARTK) errmain(LACK_OF_RIGHT_PARENT, lineNum);
    
    nextSym();
    while (symbol != LBRACETK && symbol != EOFTK)
        nextSym();

	if (deepDbg) printf("MAIN FUNCTION\n");
    insertTable(funciden, FUNCKD, VOIDTP, 0, address++, 0);

    genQuaternion(FUNCOP, oprstr[(int)VOIDOP], oprstr[(int)SPACEOP], funciden);
    if (symbol != LBRACETK) //之后是左大括号
    {
        errmain(LACK_OF_LEFT_BRACE, lineNum);
    }
    else
        nextSym();
    
    compound();
    if (symbol != RBRACETK) errmain(LACK_OF_RIGHT_BRACE, lineNum);
    else nextSym();

    if (symbol != EOFTK) serror();  //主函数后，程序结束
    if (syntaxDbg)
        printf("Line %d: This is a main function.\n", lineNum);

    checkReturnCode(inMain);
    genQuaternion(ENDOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], oprstr[(int)SPACEOP]);
}

void statementList()                                                //语句列
{
    while (symbol != RBRACETK && symbol != EOFTK)  //遇到右大括号，说明函数体结束了，返回
    {
        //否则就是语句列内的内容
        statement();
        if (symbol == EOFTK)
        {
            errmain(UNEXPETED_END_OF_FILE, lineNum);
            return ;
        }
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
            if (symbol != RBRACETK) errmain(LACK_OF_RIGHT_BRACE, lineNum);
            else nextSym();
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
            else
            {
                serror();
                while (symbol != SEMITK && symbol != EOFTK)
                    nextSym();
            }
            if (symbol != SEMITK)   //简单语句应以分号结尾
                errmain(LACK_OF_SEMICOLON, lineNum);
            else nextSym();  //预读下一个字符
            break;
        }
        case(SCANFTK):
        {
            nextSym();
            readState();
            if (symbol != SEMITK) errmain(LACK_OF_SEMICOLON, lineNum);
            else nextSym();
            break;
        }
        case(PRINTFTK):
        {
            nextSym();
            writeState();
            if (symbol != SEMITK)
            {
                errmain(LACK_OF_SEMICOLON, lineNum);

            }
            else nextSym();
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
            if (symbol != SEMITK) errmain(LACK_OF_SEMICOLON, lineNum);
            else nextSym();
            break;
        }
        default: 
        {
            serror();
            while (symbol != IFTK && symbol != ELSETK && symbol != WHILETK && symbol != LBRACETK && symbol != RBRACETK &&
                   symbol != IDENTK && symbol != SCANFTK && symbol != PRINTFTK && symbol != SEMITK && 
                   symbol != SWITCHTK && symbol != CASETK && symbol != RETURNTK && symbol != EOFTK)
                nextSym();
        }
    }
}

void ifState()
{
    string label1 = genNewLab();     //指向else的标签
    string label2 = genNewLab();     //指向下一条语句的标签

    if (symbol != LPARTK) errmain(LACK_OF_LEFT_PARENT, lineNum); //条件前应加括号
    else nextSym();
    condition();

    genQuaternion(JNEOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], label1);

    if (symbol != RPARTK) errmain(LACK_OF_RIGHT_PARENT, lineNum);
    else nextSym();
    statement();

    genQuaternion(JMPOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], label2);
    genQuaternion(LABOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], label1);

    if (symbol == ELSETK)
    {
        nextSym();
        statement();
    }
    else
        errmain(LACK_OF_ELSE_STATEMENT, lineNum);

    genQuaternion(LABOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], label2);
    //nextSym();  //预读下一个单词
    if (syntaxDbg) printf("Line %d: This is a IF statement.\n", lineNum);
}

void condition()
{
    string var1, var2;  //条件的左操作数和右操作数
    var1 = expr();  //左操作数
    if (symbol == LESTK || symbol == LEQTK || symbol == GRETK || symbol == GEQTK || symbol == NEQTK || symbol == EQUTK) //关系运算符
    {
        oprSet relop;
        switch (symbol)
        {
            case(LESTK): relop = LESOP; break;
            case(LEQTK): relop = LEQOP; break;
            case(GRETK): relop = GREOP; break;
            case(GEQTK): relop = GEQOP; break;
            case(NEQTK): relop = NEQOP; break;
            case(EQUTK): relop = EQUOP; break;
        }

        nextSym();
        var2 = expr();  //右操作数
        
        genQuaternion(relop, var1, var2, oprstr[(int)SPACEOP]);
    }
    else
    {
        //表达式为0条件为假，否则为真
        genQuaternion(NEQOP, var1, 0, oprstr[(int)SPACEOP]);
    }
    //nextSym();
}

void whileState()
{
    string label1 = genNewLab();    //条件计算前的标签
    string label2 = genNewLab();    //指向下一条语句的标签

    genQuaternion(LABOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], label1);

    if (symbol != LPARTK) errmain(LACK_OF_LEFT_PARENT, lineNum); //条件前应加括号
    else nextSym();
    condition();
    
    genQuaternion(JNEOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], label2);

    if (symbol != RPARTK) errmain(LACK_OF_RIGHT_PARENT, lineNum);
    else nextSym();
    statement();
    
    genQuaternion(JMPOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], label1);
    genQuaternion(LABOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], label2);
    //nextSym();
    if (syntaxDbg) printf("Line %d: This is a WHILE statement.\n", lineNum);
}

string funcCall()
{
    //函数调用语句，此时已预读到左括号或分号，leftiden存放函数名
    string var1, var2, var3;    //约定：var1存放最终结果
    int funcplace = searchTable(leftiden, true);
    if (funcplace == -1) errmain(UNDEFINED_IDENTITY, lineNum, leftiden);  //未定义的函数

    int para = 0;   //参数数目
    string funcname = leftiden;

    if (symbol == LPARTK)   //左括号后跟值参数表
    {
        nextSym();
        para = paramVal();
        if (symbol != RPARTK) serror(); //值参数表后接右括号
        nextSym();
    }
    //否则为无参函数调用

    if (symbolTable.item[funcplace].type != VOIDTP) var1 = genNewVar();
    else var1 = oprstr[(int)SPACEOP];
    genQuaternion(CALLOP, funcname, oprstr[(int)SPACEOP], var1);    //var1 = funcname, call, 

    if (para != symbolTable.item[funcplace].len)
        errmain(PARAMETER_COUNT_MISMATCH, lineNum, funcname);  //传参个数与函数声明中的参数个数不同，报错
    if (syntaxDbg) printf("Line %d: This is a function call statement.\n", lineNum);

    return var1;
}

int paramVal()
{
    string var1;
    vector<string> l;   //函数参数的向量表
    l.clear();
	int para = 0;
    var1 = expr(); //至少有一个表达式
    para++;
    l.push_back(var1);
    while (symbol != RPARTK && symbol != EOFTK)
    {
		if (symbol != COMMATK) serror();
        nextSym();
        var1 = expr();
        para++;
        l.push_back(var1);
    }

    //生成四元式载入对应函数参数
    for (int i = 0; i < l.size(); i++)
        genQuaternion(PARAVOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], l[i]);

    return para;
}

void assignState()
{
    //赋值语句，此时已预读到赋值符号或左中括号，leftiden存标识符
    string var1, var2, var3 = leftiden; //var3存标识符
    bool isArray = symbol == LIPARTK;

    int varplace = searchTable(leftiden, false);
    if (varplace == -1)
    {
        errmain(UNDEFINED_IDENTITY, lineNum, leftiden);   //未定义的标识符
        while (symbol != SEMITK && symbol != EOFTK)
            nextSym();
        return;
    }
    else if (symbolTable.item[varplace].kind == CONSTKD)
    {
        errmain(CONST_ASSIGNMENT, lineNum, leftiden);  //不允许向常数赋值
        while (symbol != SEMITK && symbol != EOFTK)
            nextSym();
        return;
    }

    if (symbol == LIPARTK)  //左中括号，数组元素赋值
    {
        if (symbolTable.item[varplace].len == 0)
        {
            serror(leftiden+" is not an array.");  //标识符对应符号不是数组
            while (symbol != SEMITK && symbol != EOFTK)
                nextSym();
            return;
        }
        int tarlen = symbolTable.item[varplace].len;
        nextSym();
        var1 = expr();      //var1存放数组索引
        
        //检查数组索引越界问题
        if (isInt(var1[0]))
        {
            int tind = atoi(var1.c_str());
            if (tind < 0 || tind >= tarlen)
                errmain(ARRAY_INDEX_EXCEED, lineNum, var1);
        }

        if (symbol != RIPARTK)
        {
            errmain(LACK_OF_RIGHT_BRACKET, lineNum);
            while (symbol != ASSTK && symbol != SEMITK && symbol != EOFTK)
                nextSym();
        }
        else nextSym();  //此时symbol应当为赋值符号
    }
    if (symbol != ASSTK) errmain(LACK_OF_ASSIGN_MARK, lineNum);
    else nextSym();
    var2 = expr();      //var2存放表达式右端的值
    if (var2 == "" || var2 == " ")     //表达式右端出现错误
        return;
    if (isArray && var1 != "" && var1 != " ")    //给数组赋值 []=, var2, var1, leftiden 即leftiden[var1] = var2
        genQuaternion(ASSAOP, var2, var1, var3);
    else        //否则, =, var2, , leftiden
        genQuaternion(ASSOP, var2, oprstr[(int)SPACEOP], var3); 
    if (syntaxDbg) printf("Line %d: This is a assign statement.\n", lineNum);
}

void readState()
{
    symtype vartp;

    if (symbol != LPARTK) errmain(LACK_OF_LEFT_PARENT, lineNum); //标识符前应加左括号
    nextSym();
    if (symbol != IDENTK) errmain(INVALID_IDENTITY, lineNum, token); //scanf语句至少有一个标识符
    leftiden = token;   //此时leftiden存放标识符

    place = searchTable(leftiden, false);
    if (place == -1) errmain(UNDEFINED_IDENTITY, lineNum, token);  //未定义的变量
    else if (symbolTable.item[place].kind == CONSTKD) errmain(CONST_ASSIGNMENT, lineNum, leftiden); //向常量赋值，不允许
    else if (symbolTable.item[place].len > 0) serror("Cannot scanf an array "+leftiden); //本文法读语句不支持数组读取

    vartp = symbolTable.item[place].type;
    genQuaternion(SCNFOP, oprstr[(int)vartp+2], oprstr[(int)SPACEOP], leftiden);

    nextSym();
    while (symbol == COMMATK)
    {
        nextSym();
        if (symbol != IDENTK) errmain(INVALID_IDENTITY, lineNum, token);
        leftiden = token;   //此时leftiden存放标识符

        place = searchTable(leftiden, false);
        if (place == -1) errmain(UNDEFINED_IDENTITY, lineNum, leftiden); //未定义的变量
        else if (symbolTable.item[place].kind == CONSTKD) errmain(CONST_ASSIGNMENT, lineNum, leftiden); //向常量赋值，不允许
        else if (symbolTable.item[place].len > 0) serror("Cannot scanf an array "+leftiden); //本文法读语句不支持数组读取

        vartp = symbolTable.item[place].type;
        genQuaternion(SCNFOP, oprstr[(int)vartp+2], oprstr[(int)SPACEOP], leftiden);

        nextSym();
    }
    if (symbol != RPARTK) errmain(LACK_OF_RIGHT_PARENT, lineNum);
    else nextSym();
    if (syntaxDbg) printf("Line %d: This is a read statement.\n", lineNum);
}

void writeState()
{
    string var1 = oprstr[(int)SPACEOP];     //字符串
    string var2 = oprstr[(int)SPACEOP];     //表达式

    if (symbol != LPARTK) errmain(LACK_OF_LEFT_PARENT, lineNum); //printf后应加括号
    nextSym();
    if (symbol != STRINGV)  //情况3：括号内只有表达式
    {
        var2 = expr();
        //nextSym();
    }
    else
    {
        //情况1，2：字符串，表达式 / 字符串
        var1 = getStringLab();
        constStrings.push_back(token);
        nextSym();
        if (symbol == COMMATK)
        {
            nextSym();
            var2 = expr();
        }
    }
    if (symbol != RPARTK) errmain(LACK_OF_RIGHT_PARENT, lineNum);
    else nextSym();    

    genQuaternion(PRNTOP, var1, var2, oprstr[(int)exprType+2]);
    if (syntaxDbg) printf("Line %d: This is a write statement.\n", lineNum);
}

void switchState()
{
    string var1;
    string exitlab = genNewLab();   //var1存放switch表达式值，exitlab指向下一条语句
    string startlab = genNewLab();  //start存放指向转移表开头的标签
    string deflab = "";    //default语句专用label

    vector<casestruct> casesubs;         //存放每个case子语句常量和标签

    if (symbol != LPARTK) errmain(LACK_OF_LEFT_PARENT, lineNum);
    else nextSym();
    var1 = expr();

    genQuaternion(JMPOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], startlab); //跳到状态表部分

    if (symbol != RPARTK) errmain(LACK_OF_RIGHT_PARENT, lineNum);
    else nextSym();
    if (symbol != LBRACETK) errmain(LACK_OF_LEFT_BRACE, lineNum);
    else nextSym();
    if (symbol != CASETK) errmain(LACK_OF_CASE_STATEMENT, lineNum); //情况表至少会有一个情况子语句
    else nextSym();
    casesubs = caseList(exitlab); //情况表
    if (symbol == DEFAULTTK)
    {
        nextSym();
        deflab = caseDefault(exitlab);
    }
    if (symbol != RBRACETK) errmain(LACK_OF_RIGHT_BRACE, lineNum);
    else nextSym();

    genQuaternion(LABOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], startlab);
    //以下是情况表判断部分：
    for (int i = 0; i < casesubs.size(); ++i)
    {
        genQuaternion(NEQOP, var1, casesubs[i].constval, oprstr[(int)SPACEOP]);
        //如果var1的值与常量值相等就跳转到该标签
        genQuaternion(JNEOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], casesubs[i].label);
    }

    if (deflab != "")   //若有default语句，最后直接无条件跳转至该语句
        genQuaternion(JMPOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], deflab);

    genQuaternion(LABOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], exitlab);

    if (syntaxDbg) printf("Line %d: This is a SWITCH statement.\n", lineNum);
}

vector<casestruct> caseList(string exitlab)
{
    vector<casestruct> casesubs;         //存放每个case子语句常量和标签
    casesubs.push_back(caseSubState(exitlab));
    while (symbol == CASETK)
    {
        nextSym();
        casesubs.push_back(caseSubState(exitlab));
    }
    return casesubs;
}

casestruct caseSubState(string exitlab)
{
    casestruct substruct;
    substruct.label = genNewLab();      //生成子语句的label

    genQuaternion(LABOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], substruct.label);

    if (symbol == CHARV)
        substruct.constval = (int)token[0];
    else
        substruct.constval = numericDef();
    nextSym();
    if (symbol != COLONTK) errmain(LACK_OF_COLON, lineNum);    //常量后应接冒号
    else nextSym();
    statement();
    //子语句结束后直接跳出switch
    genQuaternion(JMPOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], exitlab);

    if (syntaxDbg) printf("Line %d: This is a case-sub statement.\n", lineNum);

    return substruct;
}

string caseDefault(string exitlab)
{
    string dlab = genNewLab();

    genQuaternion(LABOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], dlab);

    if (symbol != COLONTK) errmain(LACK_OF_COLON, lineNum);
    else nextSym();
    statement();

    genQuaternion(JMPOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], exitlab);

    if (syntaxDbg) printf("Line %d: This is a case-default statement.\n", lineNum);

    return dlab;
}

void returnState()
{
    string var1 = oprstr[(int)SPACEOP];    //表达式的返回结果
    if (symbol == LPARTK)   //return后接表达式
    {
        if (isVoidFunc) errmain(UNEXPETED_RETURN_VALUE, lineNum);
        nextSym();
        var1 = expr();
        if (symbol != RPARTK) serror();
        nextSym();
    }

    if (inMain)     //main函数里的返回值代表程序终止
        genQuaternion(HALTOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], oprstr[(int)SPACEOP]);
    else
        genQuaternion(RETOP, oprstr[(int)SPACEOP], oprstr[(int)SPACEOP], var1);

    if (syntaxDbg) printf("Line %d: This is a return statement.\n", lineNum);
    
}

string expr(symtype lasttp)
{
    bool negSym = symbol == MINUSTK;    //检查第一个项前面是否有负号
    string var1, var2, var3;    //临时变量
    exprType = lasttp;           //表达式默认表示整型

    if (symbol == PLUSTK || symbol == MINUSTK)
    {
        nextSym();
    }
    var1 = term();          //当前项的结果

    if (negSym)     //有负号
    {
        var2 = genNewVar();     //生成一个新的临时变量
        genQuaternion(SUBOP, oprstr[(int)ZEROOP], var1, var2);  //var2 = 0-var1
        var1 = var2;        //此时var1与非循环的保持一致，表示当前项的结果
    }

    while (symbol == PLUSTK || symbol == MINUSTK)
    {
        int top = (int)symbol - 17;
        nextSym();
        var2 = term();
        var3 = genNewVar(); //生成新变量储存结果
        genQuaternion((oprSet)top, var1, var2, var3);  //var3 = var1 +/- var2
        var1 = var3;        //将新的结果赋值给var1，依次往复
    }
    return var1;    //var1即为最终结果
}

string term()
{
    string var1, var2, var3;
    var1 = factor();    //var1表示当前因子的结果
    while (symbol == STARTK || symbol == DIVTK)
    {
        int top = (int)symbol - 17;
        nextSym();
        var2 = factor();    //操作数2
        var3 = genNewVar(); //生成新的变量储存结果
        genQuaternion((oprSet)top, var1, var2, var3);  //var3 = var1 *// var2
        var1 = var3;    //将新的结果赋值给var1
    }
    return var1;
}

string factor()
{
    string var1 = " ", var2, var3;    //约定：var1存放最终结果
    char tvar[256];
    if (symbol == LPARTK)
    {
        //(表达式)
        nextSym();
        var1 = expr(exprType);      //var1此时存放的最终结果
        if (symbol != RPARTK) errmain(LACK_OF_RIGHT_PARENT, lineNum);
        nextSym();
    }
    else if (symbol == CHARV)
    {
        //字符类型，在表达式中以数值参与运算
        //exprType = CHARTP;
        constcval = token[0];
        var1 = int_to_str((int)constcval);
        nextSym();
    }
    else if (symbol == PLUSTK || symbol == MINUSTK || symbol == NDIGV || symbol == DIGV || symbol == UINTV)
    {
        //整数
        constival = numericDef();
        exprType = INTTP;
        var1 = int_to_str((int)constival);
        nextSym();
    }
    else
    {
        //标识符，标识符[表达式]，有返回值的函数调用(有参，无参)
        if (symbol != IDENTK)
        {
            errmain(INVALID_IDENTITY, lineNum, token);
            return " ";
        }
        leftiden = token;
        nextSym();
        //查符号表
        place = searchTable(leftiden, false);
        if (symbol == LIPARTK)
        {
            //数组，只可能是变量
            if (place == -1) errmain(UNDEFINED_IDENTITY, lineNum, leftiden);
            else if (symbolTable.item[place].len == 0) serror("Not a array of variable "+leftiden);
            else if (symbolTable.item[place].kind == CONSTKD) serror("There's no constant array "+leftiden);

            int tarlen = symbolTable.item[place].len;

            if (symbolTable.item[place].type == INTTP) exprType = INTTP;
            nextSym();
            var3 = leftiden;
            var1 = genNewVar();    //var1为最终结果
            symtype tmp = exprType;
            var2 = expr(exprType);      //var2存放数组索引
            exprType = tmp;
            
            //检查数组下标是否越界
            if (isInt(var2[0]))
            {
                int tind = atoi(var2.c_str());
                if (tind < 0 || tind >= tarlen)
                    errmain(ARRAY_INDEX_EXCEED, lineNum, var2);
            }

            genQuaternion(AASSOP, var3, var2, var1);   //Array assign取数组元素 =[] var1 = var3[var2]

            if (symbol != RIPARTK) errmain(LACK_OF_RIGHT_BRACKET, lineNum);
            nextSym();
        }
        else if (symbol == LPARTK)
        {
            //有返回值函数调用(有参)，当前符号：左括号
            //nextSym();
            place = searchTable(leftiden, true);
            if (place != -1)
            {
                if (symbolTable.item[place].type == INTTP) exprType = INTTP;
                if (symbolTable.item[place].type == VOIDTP) errmain(UNEXPETED_RETURN_VALUE, lineNum, leftiden);
            }
            var1 = funcCall();
        }
        else
        {
            //标识符或有返回值函数调用(无参)，当前符号：其他(可能是分号)
            //nextSym();
            //首先检查是否为函数
            if ((place = searchTable(leftiden, true)) != -1)
            {
                if (symbolTable.item[place].type == INTTP) exprType = INTTP;
                if (symbolTable.item[place].type == VOIDTP) errmain(UNEXPETED_RETURN_VALUE, lineNum, leftiden);
                var1 = funcCall();
            }
                
            else 
            {   //不是函数，是变量或常量
                place = searchTable(leftiden, false);

                if (place == -1) errmain(UNDEFINED_IDENTITY, lineNum, leftiden);  //都不是，报错
                else if (symbolTable.item[place].len > 0) serror("Invalid array usage "+leftiden); //此处不应出现数组
                else
                {
                    if (symbolTable.item[place].type == INTTP) exprType = INTTP;
                    if (symbolTable.item[place].kind == CONSTKD)    //是常量，直接打印值
                        var1 = int_to_str(symbolTable.item[place].value);
                    else    //变量直接取变量名
                        var1 = leftiden;
                }
            }
        }

    }

    return var1;
}
