// compiler.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

using namespace std;

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
bool symTableDbg;

ofstream midcodeout;
int codeCnt;        //中间代码条数指针
int labelCnt;       //标签数指针
int tvarCnt;        //临时变量个数
bool midcodeDbg;
QCODE midcode[MAXCODELEN];
vector<string> constStrings;	//存放打印语句需要的一些字符串

ofstream mipsfile;		//存放生成的汇编结果文件


int _tmain(int argc, _TCHAR* argv[])
{
	string inpath, asmpath;
	cout << "Extend-C0 Compiler" << endl << "Input File Path: ";
	getline(cin, inpath);
	cout << "Input File is: " << inpath << endl;
	cout << "MIPS result file path: ";
	getline(cin, asmpath);
	freopen(inpath.c_str(), "r", stdin);
	mipsfile.open(asmpath, ios::out);
	syntaxDbg = false;                                               //打印语法成分信息
    deepDbg = false;                                                 //打印更详细的信息
    midcodeDbg = false;                                              //打印四元式信息
	symTableDbg = false;											//打印符号表信息
	syntax();
	genMipsCode();
	fclose(stdin);
	mipsfile.close();
	cout << "MIPS result file generated!" << endl;
	//for (int i = 0; i < constStrings.size(); i++)
	//	cout << i << ": " << constStrings[i] << endl;
	return 0;
}

