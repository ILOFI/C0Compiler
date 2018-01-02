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
bool symTableDbg;

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
	cout << "Extend-C0 Compiler" << endl << "Input File Path: ";
	getline(cin, inpath);
	cout << "Input File is: " << inpath << endl;
	cout << "MIPS result file path: ";
	getline(cin, asmpath);
	freopen(inpath.c_str(), "r", stdin);
	mipsfile.open(asmpath, ios::out);
	syntaxDbg = false;                                               //��ӡ�﷨�ɷ���Ϣ
    deepDbg = false;                                                 //��ӡ����ϸ����Ϣ
    midcodeDbg = false;                                              //��ӡ��Ԫʽ��Ϣ
	symTableDbg = false;											//��ӡ���ű���Ϣ
	syntax();
	genMipsCode();
	fclose(stdin);
	mipsfile.close();
	cout << "MIPS result file generated!" << endl;
	//for (int i = 0; i < constStrings.size(); i++)
	//	cout << i << ": " << constStrings[i] << endl;
	return 0;
}

