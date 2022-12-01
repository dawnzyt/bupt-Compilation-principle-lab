#pragma once
#include<string>
#include<queue>
#include<cstdio>
#include<fstream>
#include<iostream>
#include<iomanip>
#include<map>
#include<vector>
#define MAX_SYMBOL_NUM 20 //�����ս��/�ս����Ŀ
#define MAX_PRODUCTION_NUM 100 //������ʽ��Ŀ
#define MAX_STACK_NUM 1000 //���(�﷨����)ջ��С
#define MAX_STINGS_NUM 20 //����ַ�����С,��չ���ַ���������ʽ�Ҳ��ַ���
#define MAX_STATE_NUM 50 //���״̬��
#define DEBUG 0 //DEBUG=1���һЩ������Ϣ,����Ŀ�淶��
#define ACC 6666 //LR1���������
using namespace std;

/* string_set�������ַ���,������ʾ��չ��������,����һЩ������in_set��add��merge��  */
struct string_set {
	int l = 0;//���ϴ�С
	int s[MAX_STINGS_NUM];//�ַ���
};
/* �ж�x�Ƿ���set�� */
bool in_set(string_set* set, int x);
/* ��x��ӵ�set�� */
void add(string_set* set, int x);
/* ��y�ϲ���x */
void merge(string_set* x, string_set* y);
/* ��ӡ���ż� */
void print_set(string_set* x, string sep, map<int, string>h);
/* ������(Ҳ�ǲ���ʽ) */
struct item {
	int u;//����ʽ���
	string_set v;//����ʽ�Ҳ���ż�
	string_set pros; //չ�����ż�
	int p = 0, who;//pΪ��ǰ׺ָ��(��[0,v.l]),who������ʽ���
};
/* ��ĸ�ֵassign������yȫ������x */
void assign_item(item* x, item* y);

/* ����淶��Ŀ���ϼ�dfa��״̬(״̬���ȼ�����Ŀ�淶��) */
struct item_set {
	item it[MAX_PRODUCTION_NUM];
	int l = 0;
};
/* �ķ���,�����ķ��Ķ���,�Ӿ�̬���ķ��õ�first���ϡ�dfa�淶����LR1������� */
class Grammer {
public:
	ifstream ifs;//�����������ķ�
	int S;//�Զ��嵥���ڵĿ�ʼ����
	int nu;//���ַ��ı��
	map<string, int>h; //hӳ��ÿ���ս���š����ս���ŵı��,���������з�����ʹ��int���͵ı�Ž���
	map<int, string>h_inv;//��ӳ���ŵ��ս��/���ս��
	int pd_num, n, m, tot;//pd_num������ʽ,n���ս����,m�����ս����,tot=n+m
	item pd[MAX_PRODUCTION_NUM];//�ķ�����ʽ����,�ķ���Ҫ�����κ�һ��V/T����������һ��V/T����ǰ׺,����Ϊ�˷���������ʽ,���������A
	string_set first[2 * MAX_SYMBOL_NUM];//�ķ�ÿ��V/T��First����
	string_set follow[2 * MAX_SYMBOL_NUM];//�ķ�ÿ��V��follow����
	item_set it_set[MAX_STATE_NUM];//dfa״̬/�淶������
	int tot_s;//״̬��Ŀ
	int f[MAX_STATE_NUM][MAX_SYMBOL_NUM * 2];//״̬ת������,f[i][j]��ʾdfa״̬i�����ַ�j����״̬f[i][j]
	int LR1[MAX_STATE_NUM][MAX_SYMBOL_NUM * 2];//LR(1)������,LR1[i][j]��ʾdfa״̬i�����ַ�j����һ��״̬�����Ϊ������������ĸ�����ʽ���й�Լ��
	bool book[MAX_STATE_NUM];//�ڹ���dfaʱ�ı������,�൱��bfs

	Grammer();
	string nxt(string& s);//����ʱ���ز���ʽ����һ���ս�/���ս��
	void init(string filename);//��ʼ���ķ�
	void init_first_set();//�����ķ�first����
	void init_follow_set();//��չ:�����ķ���follow����,�Թ����Զ����µ�LLR������(����Ŀʹ���Ե�����method) 
	void init_dfa();//�����ǰ׺dfa
	void init_LR1_table();//����LR1������

	bool is_V(int x);
	bool is_T(int x);
	void infer(item_set* s0, int x, item_set* s1);//��Ŀ��s0�����ַ�x���µ���Ŀ��s1(�������հ�����)
	void closure(item_set* is);//������Ŀ���հ�
	int find_item(item_set* is, item* it, int flag);//�ж���Ŀit�Ƿ�����Ŀ��is��,flag=0չ�����������,flag=1�����ж�չ����,�ؼ���who��pҪ���,��������������ʽ�Լ�pָ��λ��
	int find_item_set(item_set* is);//��Ŀ���Ƿ��ڵ�ǰ����Ŀ������(it_set)��,���Ƿ���index,�ú�����ֹ�ڹ���dfaʱ�ظ�������Ŀ����״̬
	void print_item_set(item_set* is,ofstream *ofs);//debug�����Ŀ��,ofs��ΪNULL��ӡ���ļ���
	void generate_graphiviz_code(string filename);//���ݻ�ǰ׺dfa�Լ�״̬ת���������ڽӱ�f���ɿ��ӻ���graphiviz����
	string pd_string(int pd_index);//����pd_index������ʽתΪstring��ʽ
};
/* �﷨������,�����ķ����塢����ջ�Ķ��� */
class Grammer_Analysis {
public:
	Grammer G;
	int top;
	int state_stack[MAX_STACK_NUM];//״̬ջ
	int symbol_stack[MAX_STACK_NUM];//����ջ
	Grammer_Analysis(string filename);
	void push(int state, int symbol);
	void get_top(int& state, int& symbol);
	void pop();
};