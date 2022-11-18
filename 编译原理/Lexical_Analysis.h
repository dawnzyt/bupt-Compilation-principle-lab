#pragma once

#include<string>
#include<cstdio>
#include<fstream>
#include<iostream>
#include<iomanip>
#include<map>
#include<vector>
#define MAX_NODES 1000
#define MAX_TOKEN_LENGTH 256
#define KEYS_NUM 63
#define OP_NUM 1000
using namespace std;
void init_mark();
class Lexical_Analysis {
public:
	ifstream ifs;
	int state;//词法分析状态
	int now_rows, id_counts, keys_counts, ArOp_counts, ReOp_counts, BitOp_counts, LogOp_counts, num_counts, Sep_counts, char_counts, string_counts, total_char, com_counts;//统计量
	int tot, err_tot;//tot-token数量; error_tot-词法错误数量
	string src;//源程序
	string rec_marks[MAX_NODES][2];//记录词法分析识别的所有token
	int rec_lines[MAX_NODES];//每个token所在行
	string errors[MAX_NODES];
	char token[MAX_TOKEN_LENGTH];
	char buf[2][MAX_TOKEN_LENGTH]/*两个输入缓冲区*/;
	int nb, pb, lpb;//pb-指向输入缓冲区下一个需要读入的字符   nb-当前pb指针所在的缓冲区0/1   lpb-上一个非空格等字符在输入缓冲区的位置,以便retract
	int token_len;
	bool end, allow_read;

	Lexical_Analysis();//词法分析类构造函数
	bool getFile(string f);//打开源文件
	bool isSlash(char c) { return c == '/'; }
	bool isDoubleQuotation(char c) { return c == '\"'; }//双引号
	bool isSingleQuotation(char c) { return  c == '\''; }//单引号
	bool isSeparator(char c) { return c == '{' || c == '}' || c == '[' || c == ']' || c == '(' || c == ')' || c == ',' || c == ';' || c == '#'; };
	bool isArOp(char c) { return c == '+' || c == '-' || c == '*' || c == '/' || c == '%'; };//算术运算符
	bool isReOp(char c) { return c == '=' || c == '<' || c == '>' || c == '!'; };//关系运算符
	bool isBitOp(char c) { return c == '&' || c == '|' || c == '^' || c == '~'; };//位运算符
	bool isLetter(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); };//字母
	bool isNum(char c) { return c >= '0' && c <= '9'; };//数字
	bool isKey(char* s);//关键字
	char get_char(int flag);//flag=1读一个字符包括' '、'\n'、'\t'、'\r'。flag=0,跳过这四个字符读一个字符。
	void retract();//pb指针回退一步
	void read_buf();//读入一个缓冲区
	bool analyze(int flag);//词法分析主程序
	void add_error(string error);//添加一个词法分析错误
	void add_token(string token_name, string value, int row);//添加一个token
	void statistics();//输出词法分析结果和统计结果
};
