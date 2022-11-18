#pragma once
#include<string>
#include<queue>
#include<cstdio>
#include<fstream>
#include<iostream>
#include<iomanip>
#include<map>
#include<vector>
#define MAX_SYMBOL_NUM 20 //最大非终结符/终结符数目
#define MAX_PRODUCTION_NUM 100 //最大产生式数目
#define MAX_STACK_NUM 1000 //最大(语法分析)栈大小
#define MAX_STINGS_NUM 20 //最大字符集大小,如展望字符集、产生式右侧字符集
#define MAX_STATE_NUM 50 //最大状态数
#define DEBUG 0 //DEBUG=1输出一些调试信息,如项目规范集
#define ACC 6666 //LR1分析表出口
using namespace std;

/* string_set定义了字符集,用来表示如展望符集合,后续一些方法如in_set、add、merge等  */
struct string_set {
	int l = 0;//集合大小
	int s[MAX_STINGS_NUM];//字符集
};
/* 判断x是否在set内 */
bool in_set(string_set* set, int x);
/* 将x添加到set内 */
void add(string_set* set, int x);
/* 将y合并给x */
void merge(string_set* x, string_set* y);
/* 打印符号集 */
void print_set(string_set* x, string sep, map<int, string>h);
/* 定义项(也是产生式) */
struct item {
	int u;//产生式左侧
	string_set v;//产生式右侧符号集
	string_set pros; //展望符号集
	int p = 0, who;//p为活前缀指针(∈[0,v.l]),who即产生式编号
};
/* 项的赋值assign操作将y全部赋给x */
void assign_item(item* x, item* y);

/* 定义规范项目集合即dfa的状态(状态即等价于项目规范集) */
struct item_set {
	item it[MAX_PRODUCTION_NUM];
	int l = 0;
};
/* 文法类,包括文法的读入,从静态的文法得到first集合、dfa规范集、LR1分析表等 */
class Grammer {
public:
	ifstream ifs;//输入流读入文法
	int S;//自定义单出口的开始符号
	int nu;//空字符的编号
	map<string, int>h; //h映射每个终结符号、非终结符号的编号,本程序所有分析均使用int类型的编号进行
	map<int, string>h_inv;//逆映射编号到终结符/非终结符
	int pd_num, n, m, tot;//pd_num个产生式,n个终结符号,m个非终结符号,tot=n+m
	item pd[MAX_PRODUCTION_NUM];//文法产生式集合,文法需要满足任何一个V/T都不是另外一个V/T的真前缀,这是为了方便读入产生式,如果存在如A
	string_set first[2 * MAX_SYMBOL_NUM];//文法每个V/T的First集合
	string_set follow[2 * MAX_SYMBOL_NUM];//文法每个V的follow集合
	item_set it_set[MAX_STATE_NUM];//dfa状态/规范集集合
	int tot_s;//状态数目
	int f[MAX_STATE_NUM][MAX_SYMBOL_NUM * 2];//状态转换函数,f[i][j]表示dfa状态i输入字符j到达状态f[i][j]
	int LR1[MAX_STATE_NUM][MAX_SYMBOL_NUM * 2];//LR(1)分析表,LR1[i][j]表示dfa状态i输入字符j的下一个状态。如果为负数则代表用哪个产生式进行规约。
	bool book[MAX_STATE_NUM];//在构造dfa时的标记数组,相当于bfs

	Grammer();
	string nxt(string& s);//读入时返回产生式的下一个终结/非终结符
	void init(string filename);//初始化文法
	void init_first_set();//计算文法first集合
	void init_follow_set();//拓展:计算文法的follow集合,以构造自顶向下的LLR分析表(本项目使用自底向上method) 
	void init_dfa();//构造活前缀dfa
	void init_LR1_table();//计算LR1分析表

	bool is_V(int x);
	bool is_T(int x);
	void infer(item_set* s0, int x, item_set* s1);//项目集s0输入字符x到新的项目集s1(不包括闭包计算)
	void closure(item_set* is);//计算项目集闭包
	int find_item(item_set* is, item* it, int flag);//判断项目it是否在项目集is中,flag=0展望符不用相等,flag=1还需判断展望符,关键是who和p要相等,决定是那条产生式以及p指针位置
	int find_item_set(item_set* is);//项目集是否在当前的项目集集合(it_set)中,若是返回index,该函数防止在构造dfa时重复定义项目集即状态
	void print_item_set(item_set* is,ofstream *ofs);//debug输出项目集,ofs不为NULL打印到文件中
	void generate_graphiviz_code(string filename);//根据活前缀dfa以及状态转换函数即邻接表f生成可视化的graphiviz代码
	string pd_string(int pd_index);//将第pd_index个产生式转为string形式
};
/* 语法分析类,包括文法定义、分析栈的定义 */
class Grammer_Analysis {
public:
	Grammer G;
	int top;
	int state_stack[MAX_STACK_NUM];//状态栈
	int symbol_stack[MAX_STACK_NUM];//符号栈
	Grammer_Analysis(string filename);
	void push(int state, int symbol);
	void get_top(int& state, int& symbol);
	void pop();
};