#include "stdafx.h"

using namespace std;

char *symbol_type_to_str(enum symbolSet type)
{
	switch(type)
	{
		SYMBOL_CASE(LBRACETK);
		SYMBOL_CASE(RBRACETK);
		SYMBOL_CASE(IFTK);
		SYMBOL_CASE(ELSETK);
		SYMBOL_CASE(WHILETK);
		SYMBOL_CASE(SWITCHTK);
		SYMBOL_CASE(CASETK);
		SYMBOL_CASE(DEFAULTTK);
		SYMBOL_CASE(SCANFTK);
		SYMBOL_CASE(PRINTFTK);
		SYMBOL_CASE(INTTK);
		SYMBOL_CASE(CHARTK);
		SYMBOL_CASE(CONSTTK);
		SYMBOL_CASE(VOIDTK);
		SYMBOL_CASE(MAINTK);
		SYMBOL_CASE(RETURNTK);
		SYMBOL_CASE(IDENTK);
		SYMBOL_CASE(CHARV);
		SYMBOL_CASE(STRINGV);
		SYMBOL_CASE(NDIGV);
		SYMBOL_CASE(DIGV);
		SYMBOL_CASE(UINTV);
		SYMBOL_CASE(PLUSTK);
		SYMBOL_CASE(MINUSTK);
		SYMBOL_CASE(STARTK);
		SYMBOL_CASE(DIVTK);
		SYMBOL_CASE(LPARTK);
		SYMBOL_CASE(RPARTK);
		SYMBOL_CASE(COMMATK);
		SYMBOL_CASE(SEMITK);
		SYMBOL_CASE(COLONTK);
		SYMBOL_CASE(ASSTK);
		SYMBOL_CASE(EQUTK);
		SYMBOL_CASE(GRETK);
		SYMBOL_CASE(LESTK);
		SYMBOL_CASE(GEQTK);
		SYMBOL_CASE(LEQTK);
		SYMBOL_CASE(NEQTK);
		SYMBOL_CASE(LIPARTK);
		SYMBOL_CASE(RIPARTK);
		SYMBOL_CASE(QTETK);
		SYMBOL_CASE(DBQTK);
		SYMBOL_CASE(UNDEF);
		SYMBOL_CASE(EOFTK);
	}
	return "???";
}

symbolSet reserver()
{
	//检查当前标识符是否为保留字
	string ttoken = token;
	transform(token.begin(), token.end(), ttoken.begin(), ::tolower);
	if (ttoken == "if") return IFTK;
	if (ttoken == "else") return ELSETK;
	if (ttoken == "while") return WHILETK;
	if (ttoken == "switch") return SWITCHTK;
	if (ttoken == "case") return CASETK;
	if (ttoken == "default") return DEFAULTTK;
	if (ttoken == "scanf") return SCANFTK;
	if (ttoken == "printf") return PRINTFTK;
	if (ttoken == "int") return INTTK;
	if (ttoken == "char") return CHARTK;
	if (ttoken == "const") return CONSTTK;
	if (ttoken == "void") return VOIDTK;
	if (ttoken == "main") return MAINTK;
	if (ttoken == "return") return RETURNTK;
	return IDENTK;
}

symbolSet otherChar(char x)
{
	if (isLBrace(x)) return LBRACETK;
	if (isRBrace(x)) return RBRACETK;
	if (isPlus(x)) return PLUSTK;
	if (isMinus(x)) return MINUSTK;
	if (isStar(x)) return STARTK;
	if (isDivi(x)) return DIVTK;
	if (isLPar(x)) return LPARTK;
	if (isRPar(x)) return RPARTK;
	if (isComma(x)) return COMMATK;
	if (isSemi(x)) return SEMITK;
	if (isColon(x)) return COLONTK;
	if (isLiPar(x)) return LIPARTK;
	if (isRiPar(x)) return RIPARTK;
	return UNDEF;
}

void error()
{
	//跳到下一个分界符(单引号，双引号，分号，逗号，空白符，EOF)
	while ((!isQuote(nowchar)) && (!isDQuote(nowchar)) && (!isSemi(nowchar)) && (!isComma(nowchar)) && (!isBlankChar(nowchar)) && (nowchar != EOF))
	{
		token += nowchar;
		nowchar = getchar();
	}
	ungetc(nowchar, stdin);
	symbol = UNDEF;
}

void getSym()
{
	token = "";
	num = 0;
	nowchar = getchar();
	while (isBlankChar(nowchar))
	{
		if (isNewline(nowchar)) lineNum++;
		nowchar = getchar();
	}
	
	if (nowchar == EOF)
		symbol = EOFTK;
	else if (isLetter(nowchar))	//以字母开头，可能是标识符或保留字
	{
		while (isLetter(nowchar) || isDigit(nowchar))
		{
			token += nowchar;
			nowchar = getchar();
		}
		ungetc(nowchar, stdin);
		symbol = reserver();
	}
	else if (isDigit(nowchar))	//以数字开头，可能是非零数字、数字或无符号整数
	{
		while (isDigit(nowchar))
		{
			token += nowchar;
			nowchar = getchar();
		}
		ungetc(nowchar, stdin);
		if (token.length() == 1)	//非零数字或数字
		{
			if (isNzDigit(token[0])) symbol = NDIGV;
			else symbol = DIGV;
			num = atoi(token.c_str());
		}
		else	//无符号整数
		{
			if (token[0] == '0')	//无符号整数不允许以0开头
				errmain(UNSIGNED_DIGIT_START_WITH_0, lineNum);
			
			symbol = UINTV;
			num = atoi(token.c_str());
		}
	}
	else if (isQuote(nowchar))	//单引号，表字符
	{
		nowchar = getchar();
		token += nowchar;
		if (!(isPlus(nowchar) || isMinus(nowchar) || (isStar(nowchar) || isDivi(nowchar) || isLetter(nowchar) || isDigit(nowchar))))
			errmain(INVALID_CHAR_TYPE, lineNum);
		if ((nowchar = getchar()) != '\'')
		{
			token += nowchar;
			errmain(LACK_OF_QUOTE, lineNum);
			error();
			return;
		}
		else
			symbol = CHARV;
	}
	else if (isDQuote(nowchar))	//双引号，表示字符串
	{
		nowchar = getchar();
		while (isStrChar(nowchar))
		{
			token += nowchar;
			nowchar = getchar();
		}
		if (!isDQuote(nowchar))		//缺少双引号，报错
			errmain(LACK_OF_DOUBLE_QUOTE, lineNum);
		else
			nowchar = getchar();

		symbol = STRINGV;
	}	
	else if (isEqu(nowchar))
	{
		token += nowchar;
		nowchar = getchar();
		if (isEqu(nowchar))
		{
			token += nowchar;
			symbol = EQUTK;
		}
		else
		{
			ungetc(nowchar, stdin);
			symbol = ASSTK;
		}
	}
	else if (isGreater(nowchar))
	{
		token += nowchar;
		nowchar = getchar();
		if (isEqu(nowchar))
		{
			token += nowchar;
			symbol = GEQTK;
		}
		else
		{
			ungetc(nowchar, stdin);
			symbol = GRETK;
		}
	}
	else if (isLess(nowchar))
	{
		token += nowchar;
		nowchar = getchar();
		if (isEqu(nowchar))
		{
			token += nowchar;
			symbol = LEQTK;
		}
		else
		{
			ungetc(nowchar, stdin);
			symbol = LESTK;
		}
	}
	else if (isNot(nowchar))
	{
		token += nowchar;
		nowchar = getchar();
		if (!isEqu(nowchar))
		{
			errmain(INVALID_WORD, lineNum);
			error();
			token += nowchar;
			return;
		}
		else
		{
			token += nowchar;
			symbol = NEQTK;
		}
	}
	else
	{
		token += nowchar;
		symbol = otherChar(nowchar);
		if (symbol == UNDEF) 
		{
			//errmain(INVALID_WORD, lineNum);
			error();
		}
	}
}

void lexical()
{
	char ch;
	cnt = 0;
	lineNum = 1;
	cout << "Count\tLine Num\tSymbol\t\tToken\t\t\tNumValue" << endl;
	while ((ch = getchar()) != EOF)
	{
		ungetc(ch, stdin);
		getSym();
		if (symbol == UNDEF)
			cout << "*" << ++cnt << "\t" << lineNum << "\t\t" << symbol_type_to_str(symbol) << "\t\t" << token << "\t\t\t" << num << "*" << endl;
		else
			cout << ++cnt << "\t" << lineNum << "\t\t" << symbol_type_to_str(symbol) << "\t\t" << token << "\t\t\t" << num << endl;
	}
}
/*
int main()
{
	string inpath;
	cout << "Lexical Analysis Program" << endl << "Input File Path: ";
	cin >> inpath;
	cout << "Input File is: " << inpath << endl;
	freopen(inpath.c_str(), "r", stdin);
	lexical();
	fclose(stdin);
	return 0;
}
*/