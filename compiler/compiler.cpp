// compiler.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

using namespace std;

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
QCODE midcode[MAXCODELEN];
vector<string> constStrings;	//��Ŵ�ӡ�����Ҫ��һЩ�ַ���

ofstream mipsfile;		//������ɵĻ�����ļ�


int _tmain(int argc, _TCHAR* argv[])
{
	string inpath, asmpath;
	cout << "Syntax Analysis Program" << endl << "Input File Path: ";
	cin >> inpath;
	cout << "Input File is: " << inpath << endl;
	cout << "MIPS result file path: ";
	cin >> asmpath;
	freopen(inpath.c_str(), "r", stdin);
	mipsfile.open(asmpath, ios::out);
	syntax();
	fclose(stdin);
	mipsfile.close();
	cout << "Syntax Analysis Finished!" << endl;
	for (int i = 0; i < constStrings.size(); i++)
		cout << i << ": " << constStrings[i] << endl;
	return 0;
}

