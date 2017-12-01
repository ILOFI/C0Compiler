using namespace std;

enum symbolSet {LBRACETK = 1, RBRACETK, IFTK, ELSETK, WHILETK, SWITCHTK, CASETK, 
				DEFAULTTK, SCANFTK, PRINTFTK, INTTK, CHARTK, CONSTTK, VOIDTK, 
				MAINTK, RETURNTK, IDENTK, CHARV, STRINGV, NDIGV, DIGV, UINTV,
				PLUSTK, MINUSTK, STARTK, DIVTK, LPARTK, RPARTK, COMMATK, SEMITK,
				COLONTK, ASSTK, EQUTK, GRETK, LESTK, GEQTK, LEQTK, NEQTK, LIPARTK,
				RIPARTK, QTETK, DBQTK, UNDEF};

#define isSpace(x) ((x) == ' ')
#define isNewline(x) ((x) == '\n')
#define isTab(x) ((x) == '\t')
#define isBlankChar(x) (isSpace(x) || isNewline(x) || isTab(x))

#define isLBrace(x) ((x) == '{')
#define isRBrace(x) ((x) == '}')

#define isLetter(x) (((x) == '_') || ((x) >= 'a' && (x) <= 'z') || ((x) >= 'A' && (x) <= 'Z'))
#define isNzDigit(x) ((x) >= '1' && (x) <= '9')
#define isDigit(x) ((x) >= '0' && (x) <= '9')

#define isPlus(x) ((x) == '+')
#define isMinus(x) ((x) == '-')
#define isStar(x) ((x) == '*')
#define isDivi(x) ((x) == '/')

#define isLPar(x) ((x) == '(')
#define isRPar(x) ((x) == ')')

#define isComma(x) ((x) == ',')
#define isSemi(x) ((x) == ';')
#define isColon(x) ((x) == ':')

#define isEqu(x) ((x) == '=')
#define isGreater(x) ((x) == '>')
#define isLess(x) ((x) == '<')
#define isNot(x) ((x) == '!')

#define isLiPar(x) ((x) == '[')
#define isRiPar(x) ((x) == ']')

#define isQuote(x) ((x) == '\'')
#define isDQuote(x) ((x) == '"')

#define SYMBOL_CASE(x) case(x): return(#x)

extern symbolSet symbol;
extern char nowchar;
extern string token;
extern int num, cnt, lineNum;
extern bool errflag;

void lexical();
void getSym();