// compiler.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

symbolSet symbol;
char nowchar;
string token;
int num, cnt, lineNum;
bool errflag;

string lastToken, variden, constiden, funciden, leftiden; //��һ����ʶ��
bool syntaxDbg, deepDbg;
symbolSet vartype, functype;
int arrlen;
symbolSet consttype;
int constival;
char constcval;

symTable symbolTable;

ofstream midcodeout;
int codeCnt;        //�м��������ָ��
int labelCnt;       //��ǩ��ָ��
int tvarCnt;        //��ʱ��������
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

