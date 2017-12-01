using namespace std;

extern symbolSet symbol;
extern char nowchar;
extern string token;
extern int num, cnt, lineNum;
extern bool errflag;

extern void getSym();

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
