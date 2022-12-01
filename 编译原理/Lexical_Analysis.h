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
	int state;//�ʷ�����״̬
	int now_rows, id_counts, keys_counts, ArOp_counts, ReOp_counts, BitOp_counts, LogOp_counts, num_counts, Sep_counts, char_counts, string_counts, total_char, com_counts;//ͳ����
	int tot, err_tot;//tot-token����; error_tot-�ʷ���������
	string src;//Դ����
	string rec_marks[MAX_NODES][2];//��¼�ʷ�����ʶ�������token
	int rec_lines[MAX_NODES];//ÿ��token������
	string errors[MAX_NODES];
	char token[MAX_TOKEN_LENGTH];
	char buf[2][MAX_TOKEN_LENGTH]/*�������뻺����*/;
	int nb, pb, lpb;//pb-ָ�����뻺������һ����Ҫ������ַ�   nb-��ǰpbָ�����ڵĻ�����0/1   lpb-��һ���ǿո���ַ������뻺������λ��,�Ա�retract
	int token_len;
	bool end, allow_read;

	Lexical_Analysis();//�ʷ������๹�캯��
	bool getFile(string f);//��Դ�ļ�
	bool isSlash(char c) { return c == '/'; }
	bool isDoubleQuotation(char c) { return c == '\"'; }//˫����
	bool isSingleQuotation(char c) { return  c == '\''; }//������
	bool isSeparator(char c) { return c == '{' || c == '}' || c == '[' || c == ']' || c == '(' || c == ')' || c == ',' || c == ';' || c == '#'; };
	bool isArOp(char c) { return c == '+' || c == '-' || c == '*' || c == '/' || c == '%'; };//���������
	bool isReOp(char c) { return c == '=' || c == '<' || c == '>' || c == '!'; };//��ϵ�����
	bool isBitOp(char c) { return c == '&' || c == '|' || c == '^' || c == '~'; };//λ�����
	bool isLetter(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); };//��ĸ
	bool isNum(char c) { return c >= '0' && c <= '9'; };//����
	bool isKey(char* s);//�ؼ���
	char get_char(int flag);//flag=1��һ���ַ�����' '��'\n'��'\t'��'\r'��flag=0,�������ĸ��ַ���һ���ַ���
	void retract();//pbָ�����һ��
	void read_buf();//����һ��������
	bool analyze(int flag);//�ʷ�����������
	void add_error(string error);//���һ���ʷ���������
	void add_token(string token_name, string value, int row);//���һ��token
	void statistics();//����ʷ����������ͳ�ƽ��
};
