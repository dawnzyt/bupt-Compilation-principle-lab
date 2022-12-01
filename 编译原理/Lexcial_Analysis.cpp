#include"Lexical_Analysis.h"
char keyWords[KEYS_NUM][MAX_TOKEN_LENGTH] = {
	"asm","do","if","return","try","auto","double","inline","short","typedef",
	"bool","dynamic_cast","int","signed","typeid","break","else","long","sizeof","typename",
	"case","enum","mutable","static","union","catch","explicit","namespace","static_cast","unsigned",
	"char","export","new","struct","using","class","extern","operator","switch","virtual",
	"const","FALSE","private","template","void","const_cast","float","protected","this","volatile",
	"continue","for","public","throw","wchar_t","default","friend","register","TRUE","while",
	"delete","goto","reinterpret_cast", };
map<string, string>hm,hm_inv;/* token<x,y> hm[x]=y,hm_inv[y]=x,��hm�ӼǺŵ�value,hm_inv��֮ */
string op_token[OP_NUM][2] = {
	/* ��������ֵ������ */
	"+","plus_op",
	"-","minus_op",
	"*","mul_op",
	"/","div_op",
	"%","mod_op",

	"+=","plus_eq_op",
	"-=","minus_eq_op",
	"*=","mul_eq_op",
	"/=","div_eq_op",
	"%=","mod_eq_op",

	"++","self_plus_op",
	"--","self_minus_op",

	"->","right_pointer_op",
	/* ��ϵ���� */

	"=","assign_op",
	"!","not_op",
	">","greater_op",
	"<","less_op",

	"==","eq_op",
	"!=","not_eq_op",
	">=","greater_or_eq_op",
	"<=","less_or_eq_op",

	"<<","shift_left_op", //ʵ����Ϊλ����,Ϊ�˷���Ŵ˴�
	">>","shift_right_op",

	"<<=","shift_left_eq_op",
	">>=","shift_right_eq_op",
	/* λ���� */
	"&","and_op",
	"|","or_op",
	"^","xor_op",

	"~","inv_op",

	"&=","and_eq_op",
	"|=","or_eq_op",
	"^=","xor_eq_op",

	"&&","and_logi_op", //ʵ����Ϊ�߼����㣬Ϊ����Ŵ˴�
	"||","or_logi_op",

	/* �ָ��� */
	"{","left_brace",
	"}","right_brace",
	"(","left_parentheses",
	")","right_parentheses",
	"[","left_bracket",
	"]","right_bracket",
	";","semicolon",
	",","comma",
	"#","well_no",
};
/* ��ʼ�����ű� */
void init_mark() {
	/* ע�⣺��Щ������ڲ������κεĿո��е��ַ� */
	cout << setfill('-') << setiosflags(ios::right) << setw(50) << "INIT_MARK" << setfill('-') << setw(50) << "" << endl;
	for (int i = 0; i < OP_NUM; i++) {
		cout << setfill(' ')<<setiosflags(ios::right);
		if (op_token[i][0] == "\0")break;
		hm_inv[op_token[i][0]] = op_token[i][1];
		hm[op_token[i][1]] = op_token[i][0];
		cout << setw(47) << hm[op_token[i][1]] << "  ";
		cout << setw(17) << hm_inv[op_token[i][0]] << '\n';
	}
}
/* �ʷ��������캯�� */
Lexical_Analysis::Lexical_Analysis() {
	 pb = MAX_TOKEN_LENGTH;
	 state = id_counts = keys_counts = ArOp_counts = LogOp_counts = tot = err_tot = com_counts =
		ReOp_counts = BitOp_counts = num_counts = Sep_counts = char_counts =string_counts= total_char = 0;
	now_rows = 1;
	end = 0;
	nb = 1;
	allow_read = 1;
	src = "";
}
/* �ж��Ƿ��ǹؼ��� */
bool Lexical_Analysis::isKey(char* s) {
	for (int i = 0; i < KEYS_NUM; i++) {
		if (!strcmp(s, keyWords[i]))return 1;
	}return 0;
}
/* �ַ�ָ�����һ�� */
void Lexical_Analysis::retract() {
	pb--;
	//if (pb)pb = (pb + MAX_TOKEN_LENGTH - 1) % MAX_TOKEN_LENGTH;
	//else {
	//	pb = (pb + MAX_TOKEN_LENGTH - 1) % MAX_TOKEN_LENGTH;
	//	nb ^= 1;
	//	allow_read = 0;//��һ�β�������뻺����,��
	//}
	if (buf[nb][pb] == '\n')now_rows--; //�����ַ��ǻ���,ͬ�����µ�ǰ������
	char c = buf[nb][pb];
	if (c != '\0' && c != ' ' && c != '\n' && c != '\t' && c != '\r')total_char--;//����ͳ������һ
}
/* ��Դ�����ļ� */
bool Lexical_Analysis::getFile(string f) {
	ifs.open(f);
	if (!ifs.is_open()) {
		cout << "�ʷ������ļ���ʧ��\n";
	}
	return ifs.is_open();
}
/* �����ݶ������뻺���� */
void Lexical_Analysis::read_buf() {
	pb = 0;
	nb ^= 1;
	/* ֮ǰ��Ҫallow_read����Ϊÿ��pb++(���ַ�)���ж�pb==MAX_TOKEN_LENGTH��read buf�Ӷ���pbΪ��һ��buf��0,��������ʱ�Ͷ����;
	���޸ĺ�����ÿ����pb++(���ַ�)ǰ�ж�pb==MAX_TOKEN_LENGTH,��������һ��buf���һ���ַ������pb=MAX_TOKEN_LENGTH,����������pb=0,�����Ҫ����,ֱ��256��1����
	�������ܳ���pb=0���˵����(��pb=MAX_TOKEN_LENGTH����)
	*/
	//if (!allow_read) { allow_read = 1; return; }//ʲôʱ�����������һ�����ʵ����һλǡ����buf�ĵ����ڶ�λʱ����ʱ����buf���һλָ���Ϊ0��read_buf,��������retract,ָ���ֻ��ˣ���˲��ɶ���
	char c;				
	int l = 0;
	while (l < MAX_TOKEN_LENGTH) {
		if (ifs >> noskipws >> c)
			buf[nb][l++] = c;
		else { end = 1; break; }
	}src += string(buf[nb]);
}
/* �����뻺�����ж�����һ���ַ�,flag=1��ʾ�������ո��ֱ�Ӷ�����һ���ַ� */
char Lexical_Analysis::get_char(int flag = 0) {
	//lpb = (pb + MAX_TOKEN_LENGTH - 1) % MAX_TOKEN_LENGTH; //����һ���ǿո��ַ�,pb-1Ϊ��λ��
	if (pb == MAX_TOKEN_LENGTH)read_buf();
	if (flag) {
		char c = buf[nb][pb++];
		if (c == '\n') now_rows++;
		if (c != '\0' && c != ' ' && c != '\n' && c != '\t' && c != '\r')total_char++;
		return c;
	}
	char c = buf[nb][pb++];
	while (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
		if (c == '\n') now_rows++;
		if (pb == MAX_TOKEN_LENGTH)read_buf();
		c = buf[nb][pb++];
	}if (c != '\0')total_char++;
	return c;
}
/* ���һ��token */
void Lexical_Analysis::add_token(string token_name, string value, int row) {
	++tot;rec_marks[tot][0] = token_name;rec_marks[tot][1] = value;rec_lines[tot] = row;
}
/* ���һ����Ϊerror�ı��� */
void Lexical_Analysis::add_error(string error) {
	errors[++err_tot] = string("lines:") + to_string(now_rows) + string(" ") + error;
}
/* ����ʷ����������ͳ�ƽ�� */
void Lexical_Analysis::statistics() {
	string title = "Statistical results";
	cout << setfill('-') << setiosflags(ios::right) << setw(50+title.length()/2) << title << setfill('-') << setw(50 - title.length() / 2) << "" << endl;
	cout << setfill(' ');
	cout.flags(ios::left);
	for (int i = 1; i <= tot; i++) {
		string token_string = string("<") + rec_marks[i][0] + string(",") + rec_marks[i][1] + string(">");
		cout <<setw(25)<< token_string<<"line:"<<rec_lines[i]<<endl;
	}
	/*total_char,id_counts, keys_counts, ArOp_counts, ReOp_counts, BitOp_counts,LogOp_counts, num_counts, Sep_counts, char_counts*/
	cout << "���������" << now_rows;
	cout << "\n�ַ�������" << total_char;
	cout << "\n��ʶ��������" << id_counts;
	cout << "\n�ؼ���������" << keys_counts;
	cout << "\n���������������" << ArOp_counts;
	cout << "\n��ϵ�����������" << ReOp_counts;
	cout << "\nλ�����������" << BitOp_counts;
	cout << "\n�߼������������" << LogOp_counts;
	cout << "\n��������(����������С�����޷�����)��" << num_counts;
	cout << "\n�ָ���������" << Sep_counts;
	cout << "\n�ַ�����������" << char_counts;
	cout << "\n�ַ���������" << string_counts;
	cout << "\nע��������" << com_counts;
	cout << endl;
	cout << setfill('-') << setiosflags(ios::right) << setw(50) << "errors" << setfill('-') << setw(50) << "" << endl;
	for (int i = 1; i <= err_tot; i++) 
		cout << errors[i] << endl;
}
/* ȫ�ֽ���һ�δʷ�����flag=1ֻʶ��һ��token */
bool Lexical_Analysis::analyze(int flag) {
	char c;
	int count = 0;
	while (1) {
		switch (state)
		{
		case 0://ʶ���´�
			if (flag>0 && count>0)return true;
			++count;
			memset(token, 0, sizeof(token));
			token_len = 0;
			c = get_char();
			token[token_len++] = c;
			if (c == '\0')return false;//��ֹ��
			if (isLetter(c) || c == '_')state = 1;
			else if (isNum(c))state = 2; // 
			else if (isArOp(c)) state = 6; // +��-��*��/��%
			else if (isReOp(c)) state = 7;//>������<��=
			else if (isBitOp(c))state = 9;//&��|��^��~
			else if (isSeparator(c))state = 10;//�ָ���
			else if (isDoubleQuotation(c)) state = 11;//˫�����ַ���,ע��˫���ſ��Ի��ж������Ų���
			else if (isSingleQuotation(c))state = 13;//�������ַ���
			else if (isSlash(c)) state = 14;//б�ܼ�ע�͡�
			break;

		case 1:// ʶ���±�ʶ�� 
			c = get_char(1);
			token[token_len++] = c;
			if (isNum(c) || isLetter(c) || c == '_') {
				state = 1;
			}
			else {
				retract();
				token[--token_len] = '\0';
				state = 0;
				if (isKey(token)) {
					keys_counts++;
					add_token("key", token, now_rows);
				}
				else {
					id_counts++;
					add_token("id", token, now_rows);
				}
			}break;
			/* ʶ��������ͷǷ����� */
		case 2://��������
			c = get_char(1);
			token[token_len++] = c;
			if (isNum(c)) state = 2;
			else if (c == '.') state = 3;//תС��
			else if (c == 'e' || c == 'E') state = 4;//תָ��
			else if (isLetter(c)||c=='_') {//[Error] unable to find numeric literal operator operator "��ʶ��"
				state = 101;//�����error
				break;
			}else {//ʶ�����
				retract();
				token[--token_len] = '\0';
				state = 0;
				num_counts++;
				add_token("num", token, now_rows);
			}
			break;
		case 3://С������
			c = get_char(1);
			token[token_len++] = c;
			if (isNum(c)) state = 3;
			else if (c == 'e' || c == 'E') state = 4; //ָ��
			else if (isLetter(c) || c == '_') {//[Error] unable to find numeric literal operator operator "��ʶ��"
				state = 101;
				break;
			}else {
				retract();
				token[--token_len] = '\0';
				state = 0;
				num_counts++;
				add_token("num", token, now_rows);
			}break;
		case 4://ָ������
			c = get_char(1);
			token[token_len++] = c;
			/* ��1e����ʽ�󲻽�����,���� */
			if (((token[token_len - 2] == 'e' || token[token_len - 2] == 'E')&&c!='+'&&c!='-') && !isNum(c)) {//[Error] exponent has no digits
				retract();
				add_error("[Error] exponent has no digits");
				state = 0;
				break;
			}
			if ((token[token_len - 2] == 'e' || token[token_len - 2] == 'E') && (c == '+' || c == '-'))state = 5;//ָ�����ֿ�ͷ������+ -����,תָ��������state
			else if (isNum(c)) state = 4;
			else if (isLetter(c) || c == '_') {//[Error] unable to find numeric literal operator operator "��ʶ��"
				state = 101;
				break;
			}else {
				retract();
				token[--token_len] = '\0';
				state = 0;
				num_counts++;
				add_token("num", token, now_rows);
			}break;
		case 5://ָ�������ֲ��֡�
			c = get_char(1);
			token[token_len++] = c;
			/* ��1e+����ʽ�󲻽�����,���� */
			if ((token[token_len - 2] == '+' || token[token_len - 2] == '-')  && !isNum(c)) {
				retract();
				add_error("[Error] exponent has no digits");
				state = 0;
				break;
			}
			if (isNum(c)) state = 5;
			else if (isLetter(c) || c == '_') {//[Error] unable to find numeric literal operator operator "��ʶ��"
				state = 101;
				break;
			}
			else {
				retract();
				token[--token_len] = '\0';
				state = 0;
				num_counts++;
				add_token("num", token, now_rows);
			}break;
		case 6://���������;ע�������//����/*ת��ע�Ͷ�Ӧ��״̬
			c = get_char(1); //�����������粻����a+ =b,- >;
			token[token_len++] = c;
			///* ʶ��numǰ��+-�� */
			//if (isNum(c) && (token[token_len - 2] == '-' || token[token_len - 2] == '+')&&(!tot||(rec_marks[tot][0]!="num"&&rec_marks[tot][1]!="id"))) {//������
			//	state = 2;
			//	break;
			//}
			if (c == '/' && token[token_len - 2] == '/')state = 14;// ˫б��ע��
			else if (c == '*' && token[token_len - 2] == '/')state = 15; // /**/ע��
			else if ((c == '+' && token[token_len - 2] == '+') || (c == '-' && token[token_len - 2] == '-')) {//�Լӡ��Լ�����
				state = 0;
				ArOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}
			else if (c == '=') { //+=��-=��
				state = 0;
				ArOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}
			else if (c == '>' && token[token_len - 2] == '-') {//��ָ��
				state = 0;
				add_token(hm_inv[token], token, now_rows);
			}
			else {//��һ���������
				retract();
				token[--token_len] = '\0';
				state = 0;
				ArOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}break;
		case 7://��ϵ�����
			c = get_char(1);
			token[token_len++] = c;
			if (c == '=') { //<=��>=��!=��== ��ϵ�ж�
				state = 0;
				ReOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}
			else if ((c == '<' && token[token_len - 2] == '<') || (c == '>' && token[token_len - 2] == '>')) state = 8;//<<��>>�������Ƶȡ����Ƶ�
			else {//��һ�����
				retract();
				token[--token_len] = '\0';
				state = 0;
				ReOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}break;
		case 8://�������Ƿ�ӵ�
			c = get_char(1);
			token[token_len++] = c;
			if (c == '=') {//���Ƶȡ����Ƶ�
				state = 0;
				BitOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}
			else { //�˴������ж��Ƿ�Ϊnum����id�������ǿ��Ա���,����Ӧ�����﷨�����������顣
				retract();
				state = 0;
				token[--token_len] = '\0';
				BitOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}break;
		case 9://λ�����
			c = get_char(1);
			token[token_len++] = c;
			if (token[token_len - 2] == '~') {//ȡ��ֻ���ǵ�һ�����,����
				retract();
				state = 0;
				token[--token_len] = '\0';
				BitOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}
			else if (c == '=') {
				state = 0;
				BitOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}
			else if ((c == '&' && token[token_len - 2] == '&') || ((c == '|' && token[token_len - 2] == '|'))) {//�߼������&& ||
				state = 0;
				LogOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}
			else { //λ����
				retract();
				state = 0;
				token[--token_len] = '\0';
				BitOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}break;
		case 10://separator�ָ���
			state = 0;
			Sep_counts++;
			add_token(hm_inv[token], token, now_rows);
			break;
		case 11://˫�����ַ���-֧�ֻ���(��\)��ת��
			c = get_char(1);
			token[token_len++] = c;
			while (c != '\\' && c != '"'&&c!='\n'&&c!='\0') {//����һ���ַ���
				c = get_char(1);
				token[token_len++] = c;
			}if (c == '\n'||c=='\0') {//[Error] missing terminating " character
				retract();
				add_error(string("[Error] missing terminating \" character"));
				state = 0;
			}else if (c == '\\')state = 12;//ת���hint����
			else {//�ַ����������
				state = 0;
				string_counts++;
				add_token("string", token, now_rows);
			}break;
		case 12://˫������\ ʶ��ת����߻���hint
			c = get_char(1);
			token[token_len++] = c;
			if (c == '\n') { // hint����
				state = 11;
				token[--token_len] = '\0';
				token[--token_len] = '\0';
			}
			else {// ת��
				state = 11;
			}break;
		case 13://�������ַ�����ֻ����һ���ַ��Ҳ�����մ���\��hint����
			c = get_char(1);
			token[token_len++] = c;
			while (c != '\\' && c != '\'' && c != '\n' && c != '\0') {
				c = get_char(1);
				token[token_len++] = c;
			}
			if (c == '\n' || c == '\0') {//[Error] missing terminating ' character
				state = 0;
				retract();
				add_error(string("[Error] missing terminating ' character"));
				break;
			}
			else if (c == '\\') {//ת��
				c = get_char(1);
				token[token_len++] = c;
				if (c == '\n'||c=='\0') {//�������ڲ�����\����,ֻ�ܽ���ת��,[Error] ' defining character does not allow line breaks
					state = 0;
					//add_error(string("[Error] 'character does not allow line breaks"));
					break;
				}
				state = 13;
				break;
			}
			else {//����
				state = 0;
				char_counts++;
				if (token_len == 2) {//[Error] empty character constant,�����ſմ�[ER]
					add_error(string("[Error] empty character constant"));
					break;
				}
				else if (token_len > 3&&!(token[token_len-3]=='\\'&&token_len==4)) {//[Error] multi-character character constant
					add_error(string("[Error] multi-character character constant"));
					break;
				}
				add_token("char", token, now_rows);
				break;
			}
		case 14:// //˫б��ע��,\hint���е��ҽ�����һ���ַ���\n��
			c = get_char(1);
			token[token_len++] = c;
			while (c != '\n'&&c!='\\'&&c!='\0') {//��β�ǻ��л���
				c = get_char(1);
				token[token_len++] = c;
			}
			if (c == '\\') {
				c = get_char(1);
				token[token_len++] = c;
				if (c == '\n' || c == '\0') {//hint����
					state = 14;
					token[--token_len] = '\0';
					token[--token_len] = '\0';
				}
				else {//����һ��\ б��
					state = 14;
				}break;
			}//��βΪ���л��߽����������ע��
			state = 0;
			cout;//ע��
			++com_counts;
			break;
		case 15:// /**/����ע��
			c = get_char(1);
			token[token_len++] = c;
			while (c != '*') {
				c = get_char(1);
				token[token_len++] = c;
			}c = get_char(1);
			token[token_len++] = c;
			if (c == '/') {//ע�ͽ���
				state = 0;
				cout;//ע��
				++com_counts;
				break;
			}
			else state = 15;//����
		case 101: {//[Error] unable to find numeric literal operator 'operator""a'
			int tmp = token_len - 1;
			c = get_char(1);
			while (isLetter(c) || c == '_'||isNum(c)) {
				token[token_len++] = c;
				c = get_char(1);
			}retract();
			state = 0;
			add_error(string("[Error] unable to find numeric literal operator \"") + string(&token[tmp]) + string("\""));
			break;
		}
		default:
			break;
		}
	}
}