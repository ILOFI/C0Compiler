using namespace std;

#ifndef _SYNTAX_H
#define _SYNTAX_H

extern symbolSet symbol;
extern char nowchar;
extern string token;
extern int num, cnt, lineNum;
extern bool errflag;

extern string lastToken, variden, constiden, funciden, leftiden; //上一个标识符
extern bool syntaxDbg, deepDbg;
extern symbolSet vartype, functype;
extern int arrlen;
extern symbolSet consttype;
extern int constival;
extern char constcval;

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

void expr();

void term();

void factor();

void statement();

void ifState();

void whileState();

void funcCall();

void assignState();

void readState();

void writeState();

void switchState();

void returnState();

void condition();

void caseList();

void caseSubState();

void caseDefault();

void paramVal();

#endif
