// compiler.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

symbolSet symbol;
char nowchar;
string token;
int num, cnt, lineNum;
bool errflag;

string lastToken, variden, constiden, funciden, leftiden; //上一个标识符
bool syntaxDbg, deepDbg;
symbolSet vartype, functype;
int arrlen;
symbolSet consttype;
int constival;
char constcval;

symTable symbolTable;

ofstream midcodeout;
int codeCnt;        //中间代码条数指针
int labelCnt;       //标签数指针
int tvarCnt;        //临时变量个数
bool midcodeDbg;

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	string inpath;
	cout << "Syntax Analysis Program" << endl << "Input File Path: ";
	cin >> inpath;
	cout << "Input File is: " << inpath << endl;
	freopen(inpath.c_str(), "r", stdin);
	syntax();
	fclose(stdin);
	cout << "Syntax Analysis Finished!" << endl;
	return 0;
}

