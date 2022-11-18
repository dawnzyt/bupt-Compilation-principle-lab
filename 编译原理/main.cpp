#include"Lexical_Analysis.h"
#include"Grammer_Analysis.h"
/* һ��ɨ����дʷ����� */
void lexical_ana() {
	Lexical_Analysis* lex = new Lexical_Analysis();
	cout << "������Դ�ļ�����" << '\n';
	char f[20] = "id.txt";
	cin >> f;
	(*lex).getFile(f);
	(*lex).analyze(0);
	(*lex).statistics();
}
/* token������дʷ����� */
void lexical_ana_single() {
	Lexical_Analysis* lex = new Lexical_Analysis();
	cout << "������Դ�ļ�����" << '\n';
	char f[20] = "id.txt";
	cin >> f;
	(*lex).getFile(f);
	while ((*lex).analyze(1)) {
		(*lex).statistics();
	}
}
/* �����﷨��������������������ķ�����
�ó������ʵʱ�Ĵʷ�����,���߽��дʷ������߽����﷨����
*/
void grammer_ana_realtime() {
	Lexical_Analysis* lex = new Lexical_Analysis();
	cout << "������������ļ���:" << '\n';
	string f = "g_exp.txt";
	cin >> f;
	(*lex).getFile(f);
	cout << "�������ķ��ļ���:" << '\n';
	f = "G.txt";
	cin >> f;
	/* ��ʼ�﷨���� */
	Grammer_Analysis* GA = new Grammer_Analysis(f);
	int state = 1, symbol = (*GA).G.nu;//x-״̬,y-����
	(*GA).push(state, symbol);
	int flag = 1;//flag=0��ʾ����Ҫ�����ַ�(����һ�����������ǹ�Լ����),flag=1��ʾ��Ҫ�ʷ�����������һ���ַ�,flag=2��ʾ�Ѿ��ǿ��ַ�
	bool rd_flag;
	int step = 0;
	/* ��ӡ�������̱�ͷ */
	cout << setfill('-') << setiosflags(ios::right) << setw(60) << "LR1_Analysis_Results" << setfill('-') << setw(40) << "" << endl;
	cout.flags(ios::left);
	cout << setfill(' ') << setw(6) << "step" << "|" << setw(20) << "states_stack" << "|" << setw(20) << "symbol_stack" << "|" << setw(10) << "in symbol" << "|" << setw(10) << "operation" << endl;
	cout << setw(6) << to_string(step) << "|" << setw(20) << to_string(1) << "|" << setw(20) << string("#") << "|" << setw(10) << "None" << "|" << setw(10) << "start to analyze" << endl;
	while (1) {
		++step;
		int in_symbol;//���β��������ַ�
		if (flag == 1) {
			rd_flag = (*lex).analyze(1);
			if (!rd_flag)flag = 2;
		}
		string token = (*lex).rec_marks[(*lex).tot][0], value = (*lex).rec_marks[(*lex).tot][1];
		if (flag != 2) {
			if (token == "num" || token == "id" || token == "key")in_symbol = (*GA).G.h[token];
			else in_symbol = (*GA).G.h[value];
		}
		else in_symbol = (*GA).G.h["#"];//���ַ�
		//cout << "step:" << step << " in_symbol:" << (*GA).G.h_inv[in_symbol] << endl;
		(*GA).get_top(state, symbol);
		int op = (*GA).G.LR1[state][in_symbol];//����action����
		if (!op) {
			cout << "grammer_analysis error!" << endl;
			break;
		}
		if (op == ACC) {
			cout << "grammer_analysis successfully!" << endl;
			break;
		}if (op > 0) {
			(*GA).push(op, in_symbol); //�������ƽ�����
			if (!flag)flag = 1;//��һ���ɶ�
		}
		else {//��Լ����,ʹ��-op����ʽ���й�Լ
			(*GA).top -= (*GA).G.pd[-op].v.l;
			int u = (*GA).G.pd[-op].u;
			(*GA).get_top(state, symbol);
			(*GA).push((*GA).G.LR1[state][u], u);//GOTO����
			if (flag != 2)flag = 0;//��һ���ʷ���������
		}
		/* �����������table */
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
/* �����﷨��������������������ķ�����
�ó����ȴʷ�����,�ٽ����﷨����
*/
void grammer_ana() {
	/* ��ʼ���ʷ������� */
	Lexical_Analysis* lex = new Lexical_Analysis();
	cout << "����������������ļ���:" << '\n';
	string f = "g_exp.txt";
	cin >> f;
	(*lex).getFile(f);
	(*lex).analyze(0);
	(*lex).statistics();
	cout << "�������ķ��ļ���:" << '\n';
	f = "G.txt";
	cin >> f;
	/* ��ʼ���﷨������ */
	Grammer_Analysis* GA = new Grammer_Analysis(f);
	int state = 1, symbol = (*GA).G.nu;//state-״̬,symbol-����
	(*GA).push(state, symbol);
	int flag = 1;//flag=0��ʾ����Ҫ�����ַ�(����һ�����������ǹ�Լ����),flag=1��ʾ��Ҫ�ʷ�����������һ���ַ�,flag=2��ʾ�Ѿ��ǿ��ַ�,��Ȼflag=2���ȼ����
	int step = 0;
	int token_p = 0;//token_pָ��ʷ�������recmarks,��ʾ��ǰ��Ҫ�����﷨����������ַ�
	cout.flags(ios::left);
	/* ����ʼ״̬δ���ַ�ת��Ϊstring��ʽ:step=0,��ʼ״̬Ϊ1,����ջΪ���ַ�#,δ���ַ�Ϊ�ʷ�������ȫ���ַ� */
	string token, symbols_to_be_read;
	for (int i = token_p + 1; i <= (*lex).tot; i++) {
		token = (*lex).rec_marks[i][0];
		if (token == "num" || token == "id")symbols_to_be_read += token;
		else symbols_to_be_read += (*lex).rec_marks[i][1];
	}symbols_to_be_read += string("#");
	int mx_length = max(symbols_to_be_read.length(), string("symbols to be read").length());//����mx_lengthΪδ���ַ�����ռ����
	/* ��ӡ�������̱�ͷ */
	cout << setfill('-') << setiosflags(ios::right) << setw(60) << "LR1_Analysis_Results" << setfill('-') << setw(40) << "" << endl;
	cout.flags(ios::left);
	cout << setfill(' ') << setw(6) << "step" << "|" << setw(20) << "states_stack" << "|" << setw(20) << "symbol_stack" << "|" << setw(mx_length) << "symbols to be read" << "|" << setw(10) << "operation" << endl;
	/* ��ӡstep=0��ʼ״̬ */
	cout << setw(6) << to_string(step) << "|" << setw(20) << to_string(1) << "|" << setw(20) << "#" << "|" << setw(mx_length) << symbols_to_be_read << "|" << setw(10) << "start to analyze" << endl;
	while (1) {
		++step;
		int in_symbol;//���β��������ַ�
		if (flag == 1) {
			if (token_p < (*lex).tot) token_p++;
			else flag = 2;
		}
		string token = (*lex).rec_marks[token_p][0], value = (*lex).rec_marks[token_p][1];
		if (flag != 2) {
			if (token == "num" || token == "id" || token == "key")in_symbol = (*GA).G.h[token];
			else in_symbol = (*GA).G.h[value];
		}
		else in_symbol = (*GA).G.h["#"];//���ַ�
		(*GA).get_top(state, symbol);
		int op = (*GA).G.LR1[state][in_symbol];//����action����
		if (!op) {//op=0,�Ƿ�����,���ʽ�����ķ�һ���淶����
			cout << "grammer_analysis error!" << endl;
			break;
		}
		if (op == ACC) {
			cout << "grammer_analysis successfully!" << endl;
			break;
		}if (op > 0) {//�������ƽ�����
			(*GA).push(op, in_symbol);
			if (!flag)flag = 1;//��һ����������ַ�
		}
		else {//��Լ����,ʹ�ñ��Ϊ-op�Ĳ���ʽ���й�Լ
			(*GA).top -= (*GA).G.pd[-op].v.l;
			int u = (*GA).G.pd[-op].u;
			(*GA).get_top(state, symbol);
			(*GA).push((*GA).G.LR1[state][u], u);//GOTO����
			if (flag != 2)flag = 0;//��һ���ʷ���������
		}

		/* �����������table */
		string states_stack = "", symbol_stack = "", symbols_to_be_read = "", operation;
		/* ��״̬ջ�ͷ���ջתΪstring */
		for (int i = 1; i <= (*GA).top; i++) {
			states_stack += to_string((*GA).state_stack[i]);
			if (i < (*GA).top)states_stack += ",";
			symbol_stack += (*GA).G.h_inv[(*GA).symbol_stack[i]];
		}/* ��operationתΪstring */
		if (op > 0)operation = string("Move ") + (*GA).G.h_inv[in_symbol] + string(" in");//�ƽ�
		else operation = string("Reduction using ") + (*GA).G.pd_string(-op);//��Լ
		/* ��symbols_to_be_readתΪstring��ʽ */
		for (int i = token_p + (flag != 0); i <= (*lex).tot; i++) {
			token = (*lex).rec_marks[i][0];
			if (token == "num" || token == "id")symbols_to_be_read += token;
			else symbols_to_be_read += (*lex).rec_marks[i][1];
		}symbols_to_be_read += string("#");
		/* ��ӡ��step��������¼ */
		cout << setw(6) << to_string(step) << "|" << setw(20) << states_stack << "|" << setw(20) << symbol_stack << "|" << setw(mx_length) << symbols_to_be_read << "|" << setw(10) << operation << endl;
	}
}
int main() {
	init_mark();//����
	//lexical_ana();
	//lexical_ana_single();
	grammer_ana();
	//grammer_ana_realtime();
	return 0;
}