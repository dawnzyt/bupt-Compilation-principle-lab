#include"Lexical_Analysis.h"
char keyWords[KEYS_NUM][MAX_TOKEN_LENGTH] = {
	"asm","do","if","return","try","auto","double","inline","short","typedef",
	"bool","dynamic_cast","int","signed","typeid","break","else","long","sizeof","typename",
	"case","enum","mutable","static","union","catch","explicit","namespace","static_cast","unsigned",
	"char","export","new","struct","using","class","extern","operator","switch","virtual",
	"const","FALSE","private","template","void","const_cast","float","protected","this","volatile",
	"continue","for","public","throw","wchar_t","default","friend","register","TRUE","while",
	"delete","goto","reinterpret_cast", };
map<string, string>hm,hm_inv;/* token<x,y> hm[x]=y,hm_inv[y]=x,即hm从记号到value,hm_inv反之 */
string op_token[OP_NUM][2] = {
	/* 算术（赋值）运算 */
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
	/* 关系运算 */

	"=","assign_op",
	"!","not_op",
	">","greater_op",
	"<","less_op",

	"==","eq_op",
	"!=","not_eq_op",
	">=","greater_or_eq_op",
	"<=","less_or_eq_op",

	"<<","shift_left_op", //实际上为位运算,为了方便放此处
	">>","shift_right_op",

	"<<=","shift_left_eq_op",
	">>=","shift_right_eq_op",
	/* 位运算 */
	"&","and_op",
	"|","or_op",
	"^","xor_op",

	"~","inv_op",

	"&=","and_eq_op",
	"|=","or_eq_op",
	"^=","xor_eq_op",

	"&&","and_logi_op", //实际上为逻辑运算，为方便放此处
	"||","or_logi_op",

	/* 分隔符 */
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
/* 初始化符号表 */
void init_mark() {
	/* 注意：这些运算符内不能有任何的空格换行等字符 */
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
/* 词法分析构造函数 */
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
/* 判断是否是关键字 */
bool Lexical_Analysis::isKey(char* s) {
	for (int i = 0; i < KEYS_NUM; i++) {
		if (!strcmp(s, keyWords[i]))return 1;
	}return 0;
}
/* 字符指针回退一步 */
void Lexical_Analysis::retract() {
	pb--;
	//if (pb)pb = (pb + MAX_TOKEN_LENGTH - 1) % MAX_TOKEN_LENGTH;
	//else {
	//	pb = (pb + MAX_TOKEN_LENGTH - 1) % MAX_TOKEN_LENGTH;
	//	nb ^= 1;
	//	allow_read = 0;//下一次不允许读入缓冲区,。
	//}
	if (buf[nb][pb] == '\n')now_rows--; //回退字符是换行,同步更新当前行数。
	char c = buf[nb][pb];
	if (c != '\0' && c != ' ' && c != '\n' && c != '\t' && c != '\r')total_char--;//回退统计量减一
}
/* 打开源代码文件 */
bool Lexical_Analysis::getFile(string f) {
	ifs.open(f);
	if (!ifs.is_open()) {
		cout << "词法分析文件打开失败\n";
	}
	return ifs.is_open();
}
/* 将数据读入输入缓冲区 */
void Lexical_Analysis::read_buf() {
	pb = 0;
	nb ^= 1;
	/* 之前需要allow_read是因为每次pb++(拿字符)后判断pb==MAX_TOKEN_LENGTH来read buf从而置pb为下一个buf的0,这样回退时就多读了;
	而修改后我们每次在pb++(读字符)前判断pb==MAX_TOKEN_LENGTH,这样对于一个buf最后一个字符读完后pb=MAX_TOKEN_LENGTH,而不会多读置pb=0,如果需要回退,直接256减1即可
	即不可能出现pb=0回退的情况(被pb=MAX_TOKEN_LENGTH代替)
	*/
	//if (!allow_read) { allow_read = 1; return; }//什么时候不允许读，即一个单词的最后一位恰好是buf的倒数第二位时，此时读入buf最后一位指针变为0会read_buf,后续进行retract,指针又回退，因此不可读。
	char c;				
	int l = 0;
	while (l < MAX_TOKEN_LENGTH) {
		if (ifs >> noskipws >> c)
			buf[nb][l++] = c;
		else { end = 1; break; }
	}src += string(buf[nb]);
}
/* 从输入缓冲区中读入下一个字符,flag=1表示不跳过空格等直接读入下一个字符 */
char Lexical_Analysis::get_char(int flag = 0) {
	//lpb = (pb + MAX_TOKEN_LENGTH - 1) % MAX_TOKEN_LENGTH; //读完一个非空格字符,pb-1为其位置
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
/* 添加一个token */
void Lexical_Analysis::add_token(string token_name, string value, int row) {
	++tot;rec_marks[tot][0] = token_name;rec_marks[tot][1] = value;rec_lines[tot] = row;
}
/* 添加一个名为error的报错 */
void Lexical_Analysis::add_error(string error) {
	errors[++err_tot] = string("lines:") + to_string(now_rows) + string(" ") + error;
}
/* 输出词法分析结果和统计结果 */
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
	cout << "语句行数：" << now_rows;
	cout << "\n字符总数：" << total_char;
	cout << "\n标识符总数：" << id_counts;
	cout << "\n关键字总数：" << keys_counts;
	cout << "\n算术运算符总数：" << ArOp_counts;
	cout << "\n关系运算符总数：" << ReOp_counts;
	cout << "\n位运算符总数：" << BitOp_counts;
	cout << "\n逻辑运算符总数：" << LogOp_counts;
	cout << "\n常数总数(包括整数、小数和无符号数)：" << num_counts;
	cout << "\n分隔符总数：" << Sep_counts;
	cout << "\n字符常数总数：" << char_counts;
	cout << "\n字符串总数：" << string_counts;
	cout << "\n注释总数：" << com_counts;
	cout << endl;
	cout << setfill('-') << setiosflags(ios::right) << setw(50) << "errors" << setfill('-') << setw(50) << "" << endl;
	for (int i = 1; i <= err_tot; i++) 
		cout << errors[i] << endl;
}
/* 全局进行一次词法分析flag=1只识别一个token */
bool Lexical_Analysis::analyze(int flag) {
	char c;
	int count = 0;
	while (1) {
		switch (state)
		{
		case 0://识别新词
			if (flag>0 && count>0)return true;
			++count;
			memset(token, 0, sizeof(token));
			token_len = 0;
			c = get_char();
			token[token_len++] = c;
			if (c == '\0')return false;//终止符
			if (isLetter(c) || c == '_')state = 1;
			else if (isNum(c))state = 2; // 
			else if (isArOp(c)) state = 6; // +、-、*、/、%
			else if (isReOp(c)) state = 7;//>、！、<、=
			else if (isBitOp(c))state = 9;//&、|、^、~
			else if (isSeparator(c))state = 10;//分隔符
			else if (isDoubleQuotation(c)) state = 11;//双引号字符串,注意双引号可以换行而单引号不行
			else if (isSingleQuotation(c))state = 13;//单引号字符串
			else if (isSlash(c)) state = 14;//斜杠即注释。
			break;

		case 1:// 识别新标识符 
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
			/* 识别符号数和非符号数 */
		case 2://整数部分
			c = get_char(1);
			token[token_len++] = c;
			if (isNum(c)) state = 2;
			else if (c == '.') state = 3;//转小数
			else if (c == 'e' || c == 'E') state = 4;//转指数
			else if (isLetter(c)||c=='_') {//[Error] unable to find numeric literal operator operator "标识符"
				state = 101;//处理该error
				break;
			}else {//识别结束
				retract();
				token[--token_len] = '\0';
				state = 0;
				num_counts++;
				add_token("num", token, now_rows);
			}
			break;
		case 3://小数部分
			c = get_char(1);
			token[token_len++] = c;
			if (isNum(c)) state = 3;
			else if (c == 'e' || c == 'E') state = 4; //指数
			else if (isLetter(c) || c == '_') {//[Error] unable to find numeric literal operator operator "标识符"
				state = 101;
				break;
			}else {
				retract();
				token[--token_len] = '\0';
				state = 0;
				num_counts++;
				add_token("num", token, now_rows);
			}break;
		case 4://指数部分
			c = get_char(1);
			token[token_len++] = c;
			/* 即1e的形式后不接数字,报错 */
			if (((token[token_len - 2] == 'e' || token[token_len - 2] == 'E')&&c!='+'&&c!='-') && !isNum(c)) {//[Error] exponent has no digits
				retract();
				add_error("[Error] exponent has no digits");
				state = 0;
				break;
			}
			if ((token[token_len - 2] == 'e' || token[token_len - 2] == 'E') && (c == '+' || c == '-'))state = 5;//指数部分开头可能有+ -区分,转指数纯整数state
			else if (isNum(c)) state = 4;
			else if (isLetter(c) || c == '_') {//[Error] unable to find numeric literal operator operator "标识符"
				state = 101;
				break;
			}else {
				retract();
				token[--token_len] = '\0';
				state = 0;
				num_counts++;
				add_token("num", token, now_rows);
			}break;
		case 5://指数纯数字部分。
			c = get_char(1);
			token[token_len++] = c;
			/* 即1e+的形式后不接数字,报错 */
			if ((token[token_len - 2] == '+' || token[token_len - 2] == '-')  && !isNum(c)) {
				retract();
				add_error("[Error] exponent has no digits");
				state = 0;
				break;
			}
			if (isNum(c)) state = 5;
			else if (isLetter(c) || c == '_') {//[Error] unable to find numeric literal operator operator "标识符"
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
		case 6://算术运算符;注意如果是//或者/*转到注释对应的状态
			c = get_char(1); //必须连续比如不允许a+ =b,- >;
			token[token_len++] = c;
			///* 识别num前的+-号 */
			//if (isNum(c) && (token[token_len - 2] == '-' || token[token_len - 2] == '+')&&(!tot||(rec_marks[tot][0]!="num"&&rec_marks[tot][1]!="id"))) {//符号数
			//	state = 2;
			//	break;
			//}
			if (c == '/' && token[token_len - 2] == '/')state = 14;// 双斜杠注释
			else if (c == '*' && token[token_len - 2] == '/')state = 15; // /**/注释
			else if ((c == '+' && token[token_len - 2] == '+') || (c == '-' && token[token_len - 2] == '-')) {//自加、自减符号
				state = 0;
				ArOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}
			else if (c == '=') { //+=、-=等
				state = 0;
				ArOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}
			else if (c == '>' && token[token_len - 2] == '-') {//右指针
				state = 0;
				add_token(hm_inv[token], token, now_rows);
			}
			else {//单一算术运算符
				retract();
				token[--token_len] = '\0';
				state = 0;
				ArOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}break;
		case 7://关系运算符
			c = get_char(1);
			token[token_len++] = c;
			if (c == '=') { //<=、>=、!=，== 关系判断
				state = 0;
				ReOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}
			else if ((c == '<' && token[token_len - 2] == '<') || (c == '>' && token[token_len - 2] == '>')) state = 8;//<<、>>或者左移等、右移等
			else {//单一运算符
				retract();
				token[--token_len] = '\0';
				state = 0;
				ReOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}break;
		case 8://左右移是否加等
			c = get_char(1);
			token[token_len++] = c;
			if (c == '=') {//左移等、右移等
				state = 0;
				BitOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}
			else { //此处可以判断是否为num或者id，若不是可以报错,但这应该是语法分析做的事情。
				retract();
				state = 0;
				token[--token_len] = '\0';
				BitOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}break;
		case 9://位运算符
			c = get_char(1);
			token[token_len++] = c;
			if (token[token_len - 2] == '~') {//取反只能是单一运算符,回退
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
			else if ((c == '&' && token[token_len - 2] == '&') || ((c == '|' && token[token_len - 2] == '|'))) {//逻辑运算符&& ||
				state = 0;
				LogOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}
			else { //位运算
				retract();
				state = 0;
				token[--token_len] = '\0';
				BitOp_counts++;
				add_token(hm_inv[token], token, now_rows);
			}break;
		case 10://separator分隔符
			state = 0;
			Sep_counts++;
			add_token(hm_inv[token], token, now_rows);
			break;
		case 11://双引号字符串-支持换行(单\)和转义
			c = get_char(1);
			token[token_len++] = c;
			while (c != '\\' && c != '"'&&c!='\n'&&c!='\0') {//读入一行字符串
				c = get_char(1);
				token[token_len++] = c;
			}if (c == '\n'||c=='\0') {//[Error] missing terminating " character
				retract();
				add_error(string("[Error] missing terminating \" character"));
				state = 0;
			}else if (c == '\\')state = 12;//转义或hint换行
			else {//字符串定义结束
				state = 0;
				string_counts++;
				add_token("string", token, now_rows);
			}break;
		case 12://双引号中\ 识别转义或者换行hint
			c = get_char(1);
			token[token_len++] = c;
			if (c == '\n') { // hint换行
				state = 11;
				token[--token_len] = '\0';
				token[--token_len] = '\0';
			}
			else {// 转义
				state = 11;
			}break;
		case 13://单引号字符串，只允许一个字符且不允许空串且\不hint换行
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
			else if (c == '\\') {//转义
				c = get_char(1);
				token[token_len++] = c;
				if (c == '\n'||c=='\0') {//单引号内不允许\换行,只能进行转义,[Error] ' defining character does not allow line breaks
					state = 0;
					//add_error(string("[Error] 'character does not allow line breaks"));
					break;
				}
				state = 13;
				break;
			}
			else {//结束
				state = 0;
				char_counts++;
				if (token_len == 2) {//[Error] empty character constant,单引号空串[ER]
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
		case 14:// //双斜杠注释,\hint换行当且仅当下一个字符是\n。
			c = get_char(1);
			token[token_len++] = c;
			while (c != '\n'&&c!='\\'&&c!='\0') {//结尾是换行或者
				c = get_char(1);
				token[token_len++] = c;
			}
			if (c == '\\') {
				c = get_char(1);
				token[token_len++] = c;
				if (c == '\n' || c == '\0') {//hint换行
					state = 14;
					token[--token_len] = '\0';
					token[--token_len] = '\0';
				}
				else {//单纯一个\ 斜杠
					state = 14;
				}break;
			}//结尾为换行或者结束符则结束注释
			state = 0;
			cout;//注释
			++com_counts;
			break;
		case 15:// /**/段落注释
			c = get_char(1);
			token[token_len++] = c;
			while (c != '*') {
				c = get_char(1);
				token[token_len++] = c;
			}c = get_char(1);
			token[token_len++] = c;
			if (c == '/') {//注释结束
				state = 0;
				cout;//注释
				++com_counts;
				break;
			}
			else state = 15;//继续
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