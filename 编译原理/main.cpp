#include"Lexical_Analysis.h"
#include"Grammer_Analysis.h"
/* 一次扫描进行词法分析 */
void lexical_ana() {
	Lexical_Analysis* lex = new Lexical_Analysis();
	cout << "请输入源文件名：" << '\n';
	char f[20] = "id.txt";
	cin >> f;
	(*lex).getFile(f);
	(*lex).analyze(0);
	(*lex).statistics();
}
/* token级别进行词法分析 */
void lexical_ana_single() {
	Lexical_Analysis* lex = new Lexical_Analysis();
	cout << "请输入源文件名：" << '\n';
	char f[20] = "id.txt";
	cin >> f;
	(*lex).getFile(f);
	while ((*lex).analyze(1)) {
		(*lex).statistics();
	}
}
/* 进行语法分析包括分析符读入和文法读入
该程序进行实时的词法分析,即边进行词法分析边进行语法分析
*/
void grammer_ana_realtime() {
	Lexical_Analysis* lex = new Lexical_Analysis();
	cout << "请输入分析符文件名:" << '\n';
	string f = "g_exp.txt";
	cin >> f;
	(*lex).getFile(f);
	cout << "请输入文法文件名:" << '\n';
	f = "G.txt";
	cin >> f;
	/* 开始语法分析 */
	Grammer_Analysis* GA = new Grammer_Analysis(f);
	int state = 1, symbol = (*GA).G.nu;//x-状态,y-符号
	(*GA).push(state, symbol);
	int flag = 1;//flag=0表示不需要读入字符(即上一步操作可能是归约操作),flag=1表示需要词法分析读入下一个字符,flag=2表示已经是空字符
	bool rd_flag;
	int step = 0;
	/* 打印分析过程表头 */
	cout << setfill('-') << setiosflags(ios::right) << setw(60) << "LR1_Analysis_Results" << setfill('-') << setw(40) << "" << endl;
	cout.flags(ios::left);
	cout << setfill(' ') << setw(6) << "step" << "|" << setw(20) << "states_stack" << "|" << setw(20) << "symbol_stack" << "|" << setw(10) << "in symbol" << "|" << setw(10) << "operation" << endl;
	cout << setw(6) << to_string(step) << "|" << setw(20) << to_string(1) << "|" << setw(20) << string("#") << "|" << setw(10) << "None" << "|" << setw(10) << "start to analyze" << endl;
	while (1) {
		++step;
		int in_symbol;//本次操作输入字符
		if (flag == 1) {
			rd_flag = (*lex).analyze(1);
			if (!rd_flag)flag = 2;
		}
		string token = (*lex).rec_marks[(*lex).tot][0], value = (*lex).rec_marks[(*lex).tot][1];
		if (flag != 2) {
			if (token == "num" || token == "id" || token == "key")in_symbol = (*GA).G.h[token];
			else in_symbol = (*GA).G.h[value];
		}
		else in_symbol = (*GA).G.h["#"];//空字符
		//cout << "step:" << step << " in_symbol:" << (*GA).G.h_inv[in_symbol] << endl;
		(*GA).get_top(state, symbol);
		int op = (*GA).G.LR1[state][in_symbol];//包括action操作
		if (!op) {
			cout << "grammer_analysis error!" << endl;
			break;
		}
		if (op == ACC) {
			cout << "grammer_analysis successfully!" << endl;
			break;
		}if (op > 0) {
			(*GA).push(op, in_symbol); //正常的移进操作
			if (!flag)flag = 1;//下一步可读
		}
		else {//归约操作,使用-op产生式进行归约
			(*GA).top -= (*GA).G.pd[-op].v.l;
			int u = (*GA).G.pd[-op].u;
			(*GA).get_top(state, symbol);
			(*GA).push((*GA).G.LR1[state][u], u);//GOTO操作
			if (flag != 2)flag = 0;//下一步词法分析不读
		}
		/* 输出分析过程table */
		string states_stack = "", symbol_stack = "", symbols, operation;
		for (int i = 1; i <= (*GA).top; i++) {
			states_stack += to_string((*GA).state_stack[i]);
			if (i < (*GA).top)states_stack += ",";
			symbol_stack += (*GA).G.h_inv[(*GA).symbol_stack[i]];
		}if (op > 0)operation = string("Move ") + (*GA).G.h_inv[in_symbol] + string(" in");
		else operation = string("Reduction using ") + (*GA).G.pd_string(-op);
		cout << setw(6) << to_string(step) << "|" << setw(20) << states_stack << "|" << setw(20) << symbol_stack << "|" << setw(10) << (*GA).G.h_inv[in_symbol] << "|" << setw(10) << operation << endl;
	}
}
/* 进行语法分析包括分析符读入和文法读入
该程序先词法分析,再进行语法分析
*/
void grammer_ana() {
	/* 初始化词法分析类 */
	Lexical_Analysis* lex = new Lexical_Analysis();
	cout << "请输入分析串所在文件名:" << '\n';
	string f = "g_exp.txt";
	cin >> f;
	(*lex).getFile(f);
	(*lex).analyze(0);
	(*lex).statistics();
	cout << "请输入文法文件名:" << '\n';
	f = "G.txt";
	cin >> f;
	/* 初始化语法分析类 */
	Grammer_Analysis* GA = new Grammer_Analysis(f);
	int state = 1, symbol = (*GA).G.nu;//state-状态,symbol-符号
	(*GA).push(state, symbol);
	int flag = 1;//flag=0表示不需要读入字符(即上一步操作可能是归约操作),flag=1表示需要词法分析读入下一个字符,flag=2表示已经是空字符,显然flag=2优先级最高
	int step = 0;
	int token_p = 0;//token_p指向词法分析的recmarks,表示当前需要进行语法分析输入的字符
	cout.flags(ios::left);
	/* 将初始状态未读字符转化为string格式:step=0,初始状态为1,符号栈为空字符#,未读字符为词法分析的全部字符 */
	string token, symbols_to_be_read;
	for (int i = token_p + 1; i <= (*lex).tot; i++) {
		token = (*lex).rec_marks[i][0];
		if (token == "num" || token == "id")symbols_to_be_read += token;
		else symbols_to_be_read += (*lex).rec_marks[i][1];
	}symbols_to_be_read += string("#");
	int mx_length = max(symbols_to_be_read.length(), string("symbols to be read").length());//定义mx_length为未读字符块所占长度
	/* 打印分析过程表头 */
	cout << setfill('-') << setiosflags(ios::right) << setw(60) << "LR1_Analysis_Results" << setfill('-') << setw(40) << "" << endl;
	cout.flags(ios::left);
	cout << setfill(' ') << setw(6) << "step" << "|" << setw(20) << "states_stack" << "|" << setw(20) << "symbol_stack" << "|" << setw(mx_length) << "symbols to be read" << "|" << setw(10) << "operation" << endl;
	/* 打印step=0起始状态 */
	cout << setw(6) << to_string(step) << "|" << setw(20) << to_string(1) << "|" << setw(20) << "#" << "|" << setw(mx_length) << symbols_to_be_read << "|" << setw(10) << "start to analyze" << endl;
	while (1) {
		++step;
		int in_symbol;//本次操作输入字符
		if (flag == 1) {
			if (token_p < (*lex).tot) token_p++;
			else flag = 2;
		}
		string token = (*lex).rec_marks[token_p][0], value = (*lex).rec_marks[token_p][1];
		if (flag != 2) {
			if (token == "num" || token == "id" || token == "key")in_symbol = (*GA).G.h[token];
			else in_symbol = (*GA).G.h[value];
		}
		else in_symbol = (*GA).G.h["#"];//空字符
		(*GA).get_top(state, symbol);
		int op = (*GA).G.LR1[state][in_symbol];//包括action操作
		if (!op) {//op=0,非法操作,表达式并非文法一个规范句型
			cout << "grammer_analysis error!" << endl;
			break;
		}
		if (op == ACC) {
			cout << "grammer_analysis successfully!" << endl;
			break;
		}if (op > 0) {//正常的移进操作
			(*GA).push(op, in_symbol);
			if (!flag)flag = 1;//下一步需读入新字符
		}
		else {//归约操作,使用编号为-op的产生式进行归约
			(*GA).top -= (*GA).G.pd[-op].v.l;
			int u = (*GA).G.pd[-op].u;
			(*GA).get_top(state, symbol);
			(*GA).push((*GA).G.LR1[state][u], u);//GOTO操作
			if (flag != 2)flag = 0;//下一步词法分析不读
		}

		/* 输出分析过程table */
		string states_stack = "", symbol_stack = "", symbols_to_be_read = "", operation;
		/* 将状态栈和符号栈转为string */
		for (int i = 1; i <= (*GA).top; i++) {
			states_stack += to_string((*GA).state_stack[i]);
			if (i < (*GA).top)states_stack += ",";
			symbol_stack += (*GA).G.h_inv[(*GA).symbol_stack[i]];
		}/* 将operation转为string */
		if (op > 0)operation = string("Move ") + (*GA).G.h_inv[in_symbol] + string(" in");//移进
		else operation = string("Reduction using ") + (*GA).G.pd_string(-op);//归约
		/* 将symbols_to_be_read转为string形式 */
		for (int i = token_p + (flag != 0); i <= (*lex).tot; i++) {
			token = (*lex).rec_marks[i][0];
			if (token == "num" || token == "id")symbols_to_be_read += token;
			else symbols_to_be_read += (*lex).rec_marks[i][1];
		}symbols_to_be_read += string("#");
		/* 打印第step条分析记录 */
		cout << setw(6) << to_string(step) << "|" << setw(20) << states_stack << "|" << setw(20) << symbol_stack << "|" << setw(mx_length) << symbols_to_be_read << "|" << setw(10) << operation << endl;
	}
}
int main() {
	init_mark();//必须
	//lexical_ana();
	//lexical_ana_single();
	grammer_ana();
	//grammer_ana_realtime();
	return 0;
}