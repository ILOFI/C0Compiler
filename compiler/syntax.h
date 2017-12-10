using namespace std;

#ifndef _SYNTAX_H
#define _SYNTAX_H

#include "symtable.h"

extern symbolSet symbol;
extern char nowchar;
extern string token;
extern int num, cnt, lineNum;
extern bool errflag;

extern string lastToken, variden, constiden, funciden, leftiden; //上一个标识符
extern bool syntaxDbg, deepDbg;
extern bool symTableDbg;
extern symbolSet vartype, functype;
extern int arrlen;
extern symbolSet consttype;
extern int constival;
extern char constcval;

typedef struct
{
    int constval;
    string label;
} casestruct;   //case子语句的结构

extern void getSym();

extern char *symbol_type_to_str(enum symbolSet type);

void nextSym();

void syntax();

void program();

void constDec();

void constDef();

int numericDef();

void decHead();

void varDec();

void varDef();

void funcDef();

void paramList();

void compound();

void statementList();

void mainFunc();

string expr(enum symtype lasttp = INTTP);

string term();

string factor();

void statement();

void ifState();

void whileState();

string funcCall();

void assignState();

void readState();

void writeState();

void switchState();

void returnState();

void condition();

vector<casestruct> caseList(string exitlab);

casestruct caseSubState(string exitlab);

string caseDefault(string exitlab);

int paramVal();

#endif

