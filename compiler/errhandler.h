#ifndef _ERRHANDLER_H
#define _ERRHANDLER_H

#include <string>
using namespace std;

enum errType {
    UNSIGNED_DIGIT_START_WITH_0,    //无符号整数出现前导0
    INVALID_CHAR_TYPE,              //非法字符类型
    LACK_OF_QUOTE,                  //缺少单引号
    LACK_OF_DOUBLE_QUOTE,           //缺少双引号
    SYMBOL_TABLE_FULL,              //符号表满
    INVALID_IDENTITY,               //非法标识符
    DUPLICATED_DEFINE_IDENTITY,     //标识符多重定义
    UNDEFINED_IDENTITY,             //未定义的标识符
    CONST_NOT_ASSIGNED,             //常量未赋值
    INVALID_CONST_VALUE,            //常量值异常
    INVALID_ARRAY_LENGTH,           //数组长度声明异常
    INVALID_TYPE,                   //变量/常量类型错误
    UNEXPETED_RETURN_VALUE,         //void型函数返回值
    LACK_OF_SEMICOLON,              //缺少分号
    LACK_OF_RIGHT_BRACKET,          //缺少右中括号
    LACK_OF_LEFT_PARENT,            //缺少左小括号
    LACK_OF_RIGHT_PARENT,           //缺少右小括号
    LACK_OF_LEFT_BRACE,             //缺少左大括号
    LACK_OF_RIGHT_BRACE,            //缺少右大括号
    LACK_OF_COLON,                  //缺少冒号
    CONST_ASSIGNMENT,               //给常量赋值
    MAIN_TYPE_ERROR,                //main函数不为void类型
    PARAMETER_COUNT_MISMATCH,       //参数个数不匹配
    LACK_OF_CASE_STATEMENT,         //缺少case子语句
    UNEXPETED_END_OF_FILE,          //文件异常结束
    LACK_OF_ASSIGN_MARK,            //缺少赋值符号

};

const string errMsg[] = {
    "UNSIGNED_DIGIT_START_WITH_0",    //无符号整数出现前导0
    "INVALID_CHAR_TYPE",              //非法字符类型
    "LACK_OF_QUOTE",                  //缺少单引号
    "LACK_OF_DOUBLE_QUOTE",           //缺少双引号
    "SYMBOL_TABLE_FULL",              //符号表满
    "INVALID_IDENTITY",               //非法标识符
    "DUPLICATED_DEFINE_IDENTITY",     //标识符多重定义
    "UNDEFINED_IDENTITY",             //未定义的标识符
    "CONST_NOT_ASSIGNED",             //常量未赋值
    "INVALID_CONST_VALUE",
    "INVALID_ARRAY_LENGTH",
    "INVALID_TYPE",
    "UNEXPETED_RETURN_VALUE_FUNCTION",//void型函数返回值
    "LACK_OF_SEMICOLON",              //缺少分号
    "LACK_OF_RIGHT_BRACKET",          //缺少右中括号
    "LACK_OF_LEFT_PARENT",            //缺少左小括号
    "LACK_OF_RIGHT_PARENT",           //缺少右小括号
    "LACK_OF_LEFT_BRACE",             //缺少左大括号
    "LACK_OF_RIGHT_BRACE",            //缺少右大括号
    "LACK_OF_COLON",                  //缺少冒号
    "CONST_ASSIGNMENT",               //给常量赋值
    "MAIN_TYPE_ERROR",                //main函数不为void类型
    "PARAMETER_COUNT_MISMATCH",       //参数个数不匹配
    "LACK_OF_CASE_STATEMENT",         //缺少case子语句
    "UNEXPETED_END_OF_FILE",          //文件异常结束
    "LACK_OF_ASSIGN_MARK",            //缺少赋值符号
};

void errmain();

void errmain(errType, int);

void errmain(errType, int, string);

#endif

//    INVALID_WORD,                   //无法识别的单词种类
//    LACK_OF_LEFT_BRACKET,           //缺少左中括号
//    PARAMETER_TYPE_MISMATCH,        //传参类型不匹配
//    "INVALID_WORD",                   //无法识别的单词种类
//    "LACK_OF_LEFT_BRACKET",           //缺少左中括号
//    "PARAMETER_TYPE_MISMATCH",        //传参类型不匹配
