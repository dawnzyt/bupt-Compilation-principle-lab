#include"Grammer_Analysis.h"
/* 将x添加到set内 */
void add(string_set *set, int x) {
	for (int i = 1; i <= set->l; i++) {
		if (set->s[i] == x)return ;
	}set->s[++set->l] = x;
}
/* 判断x是否在集合内 */
bool in_set(string_set *set, int x) {
	for (int i = 1; i <= set->l; i++) {
		if (set->s[i] == x)return true;
	}
	return false;
}
/* 将y合并给x */
void merge(string_set* x, string_set *y) {
	for (int i = 1; i <= y->l; i++) {
		if (in_set(x, y->s[i]))continue;
		x->s[++x->l] = y->s[i];
	}
}
/* 打印符号集 */
void print_set(string_set *x,string sep,map<int,string>h) {
	for (int i = 1; i <= x->l; i++) {
		cout << h[x->s[i]];
		if (i < x->l)cout << sep;
	}
	cout << endl;
}
/* 项的赋值assign操作 */
void assign_item(item* x, item* y) {
	x->u = y->u;
	x->p = y->p;
	x->who = y->who;
	x->v.l = y->v.l;
	x->pros.l = y->pros.l;
	for (int i = 1; i <= y->v.l; i++)x->v.s[i] = y->v.s[i];
	for (int i = 1; i <= y->pros.l; i++)x->pros.s[i] = y->pros.s[i];
}
/* 获得s的下一个终结符/非终结符,s为空则返回空 */
string Grammer::nxt(string& s) {
	if (s == "\0")return "\0";
	string t;
	int i = 0;
	if (i + 1 < s.length() && s[i] == '-' && s[i + 1] == '>') { i += 2; }//->
	for (int j = i; j < s.length(); j++) {
		t = s.substr(i, j - i + 1);
		if (h[t]) {//是终结符/非终结符
			s = j == s.length() - 1 ? "\0" : s.substr(j + 1, s.length() - j - 1);
			return t;
		}
	}
}
/* 文法构造函数 */
Grammer::Grammer() {
	pd_num = 0; tot = 0; tot_s = 0;
	memset(f, 0, sizeof(f)); 
	memset(book, 0, sizeof(book));
	memset(LR1, 0, sizeof(LR1));
}
/* 读入文法并初始化包括符号->int的映射、产生式的读入以及调用init_first_set、init_dfa等 */
void Grammer::init(string filename) {
	ifs.open(filename);
	if (!ifs.is_open()) {
		cout << "文法文件打开失败";
		return;
	}
	string tmp;
	ifs >> tmp;
	ifs >> n;
	string s;
	for (int i = 1; i <= n; i++) {//读入终结符
		ifs >> s;
		h[s] = ++tot;
		h_inv[tot] = s;
	}if (!h["#"]) {
		h["#"] = ++tot;//空字符也要定义
		h_inv[tot] = "#";
		++n;
	}nu = h["#"];
	ifs >> m;
	for (int i = 1; i <= m; i++) {//读入非终结符
		ifs >> s;
		h[s] = ++tot;
		h_inv[tot] = s;
	}h["S"] = ++tot; h_inv[tot] = "S"; S = h["S"]; ++m;
	/* 新增产生式,"S"->S,得到拓广文法G',保证G'归约时单入口 */
	pd_num++;
	pd[pd_num].u = h["S"]; 
	pd[pd_num].v.s[++pd[pd_num].v.l] = h[tmp];
	pd[pd_num].who = pd_num;
	/* 读入产生式 */
	while (ifs >> s) {
		++pd_num;
		pd[pd_num].u = h[nxt(s)];
		string x = nxt(s);
		while (x != "\0") {
			pd[pd_num].v.s[++pd[pd_num].v.l] = h[x];
			x = nxt(s);
		}
		pd[pd_num].who = pd_num;
	}
	/* 打印符号与编号映射关系 */
	cout << setfill('-') << setiosflags(ios::right) << setw(50) << "map" << setfill('-') << setw(50) << "" << endl;
	cout.flags(ios::left);
	for (int i = 1; i <= tot; i++) {
		cout <<setfill(' ')<< setw(4) << h_inv[i] << h[h_inv[i]] << endl;
	}
	/* 打印产生式 */
	cout << setfill('-') << setiosflags(ios::right) << setw(50) << "PRODUCTION" << setfill('-') << setw(50) << "" << endl;
	for (int i = 1; i <= pd_num; i++) {
		cout <<"r"<< i<<" "<<h_inv[pd[i].u] << "->";
		for (int j = 1; j <= pd[i].v.l; j++)
			cout << h_inv[pd[i].v.s[j]];
		cout << endl;
	}
	init_first_set();//计算first集
	init_follow_set();//计算follow集
	init_dfa();//构造活前缀dfa和状态转换函数f
	init_LR1_table();//计算LR1分析表
	generate_graphiviz_code("gph_code.txt");//生成dfa图的graphiviz代码
}
/* 计算文法的follow集合
* 首先将空字符#并入开始符号S的follow集中
* 对于任意产生式A->αBβ,将first(β)/{ε}并入B的follow集中
* 对于A->B1B2...Bn
* 若Bn不为nu将A的follow集并入Bn中;若Bi+1...Bn的first集包括空字符nu,将A的follow集并入Bi中
* 直到所有V的follow集合不增大
*/
void Grammer::init_follow_set() {
	add(&follow[S], nu);//空字符
	/* 对于任意产生式A->αBβ,将first(β)/{ε}并入B的follow集中 */
	for (int i = 1; i <= pd_num; i++) {
		item* p = &pd[i];
		for (int j = 1; j <= p->v.l; j++) {
			int A = p->v.s[j];
			if (is_V(A)) {
				string_set* first_beta = new string_set();
				for (int k = j + 1; k <= p->v.l; k++) {
					merge(first_beta, &first[p->v.s[k]]);
					if (!in_set(&first[p->v.s[k]], nu))break;
				}
				merge(&follow[A], first_beta);
			}
		}
	}
	/* A->B,将A的follow集并入B */
	bool flag = 1;
	while (flag) {
		flag = 0;
		for (int i = 1; i <= pd_num; i++) {
			item* p = &pd[i];
			for (int j = p->v.l; j; j--) {
				if (is_T(p->v.s[j]))break;
				int pre_l = follow[p->v.s[j]].l;
				merge(&follow[p->v.s[j]], &follow[p->u]);
				if (follow[p->v.s[j]].l > pre_l)flag = 1;
				if (!in_set(&first[p->v.s[j]], nu))break;
			}
;		}
	}/* 输出follow集 */
	cout << setfill('-') << setiosflags(ios::right) << setw(50) << "Follow" << setfill('-') << setw(50) << "" << endl;
	for (int i = 1; i <= tot; i++) {
		cout << "follow[" << h_inv[i] << "]=";
		print_set(&follow[i], ",", h_inv);
	}
	
}
/* 计算文法的first集合
对于A->B1B2...Bn产生式(Bi可以为终结符或者非终结符)：
1.将B1的first集合加到A中;
2.如果B1...Bi-1的first集都包括#(空字符),将Bi的first集合加入A的first集中
重复执行直到文法first集合饱和
*/
void Grammer::init_first_set() {
	for (int i = 1; i <= n; i++)add(&first[i], i);
	bool flag = 1;//flag=1表示first集未饱和
	while (flag) {
		flag = 0;
		for (int i = 1; i <= pd_num; i++) {
			for (int j = 1; j <= pd[i].v.l; j++) {
				int pre_l = first[pd[i].u].l;
				merge(&first[pd[i].u], &first[pd[i].v.s[j]]);
				flag |= first[pd[i].u].l > pre_l;//元素增加
				if (!in_set(&first[pd[i].v.s[j]], nu))break;
			}
		}
	}
	/* 输出first集 */
	cout << setfill('-') << setiosflags(ios::right) << setw(50) << "FIRST" << setfill('-') << setw(50) << "" << endl;
	for (int i = 1; i <= tot; i++) {
		cout << "first[" << h_inv[i] << "]=";
		print_set(&first[i], ",",h_inv);
	}
}
/* 判断是非终结师傅 */
bool Grammer::is_V(int x) { return x > n; }
/* 判断是终结符 */
bool Grammer::is_T(int x) { return x <= n; }
/* 判断项目it是否在项目集is中,flag=0不考虑展望符,flag=1还需判断展望符,关键是who和p要相等,决定是那条产生式以及活前缀指针位置 */
int Grammer::find_item(item_set *is, item *it,int flag) {
	for (int i = 1; i <= is->l; i++) {
		item *tmp = &is->it[i];
		if (tmp->p == it->p && tmp->who == it->who) {
			if(!flag)return i;
			/* 判断展望符是否完全相等 */
			bool fflag = 1;
			if (tmp->pros.l != it->pros.l)continue;
			for (int j = 1; j <= it->pros.l; j++) {
				if (!in_set(&tmp->pros, it->pros.s[j])) { fflag = 0; break; }
			}if (fflag)return i;
		}

	}return 0;
}
/* debug输出项目集 */
void Grammer::print_item_set(item_set *is,ofstream *ofs=NULL) {
	for (int i = 1; i <= is->l; i++) {
		item *it = &is->it[i];
 		if(ofs==NULL)cout << h_inv[it->u] << "->";
		else *ofs << h_inv[it->u] << "->";
		for (int j = 0; j <= it->v.l; j++) {
			if (j) {
				if (ofs == NULL)cout << h_inv[it->v.s[j]];
				else *ofs << h_inv[it->v.s[j]];
			}
			if (it->p == j) {
				if (ofs == NULL)cout << "·";
				else *ofs << "·";
			}
		}
		if (ofs == NULL)cout << "，";
		else *ofs << "，";
		for (int j = 1; j <= it->pros.l; j++) {
			if(ofs==NULL)cout << h_inv[it->pros.s[j]];
			else *ofs<< h_inv[it->pros.s[j]];
			if (j < it->pros.l) {
				if (ofs == NULL)cout << ",";
				else *ofs << ",";
			}
		}if (ofs == NULL)cout << endl;
		else *ofs << endl;
	}
}
/* 计算项目集闭包,方法:
项目集一个项目it为[A->α·Bβ,a],有产生式B->·γ,则添加新项目为[B->·γ,first(βa)],直至饱和
注意:first(ABC)和求first集合类似
*/
void Grammer::closure(item_set* is) {
	bool flag = 1;//flag=1表示项目集未饱和
	while (flag) {
		flag = 0;
		for (int i = 1; i <= is->l; i++) {
			item *it = &is->it[i];
			int u;
			if (it->p < it->v.l && is_V(u = it->v.s[it->p + 1])) {//A->α·uβ
				for (int j = 1; j <= pd_num; j++) {
					if (pd[j].u == u) {//u->·γ
						int idx = find_item(is, &pd[j],0);//判断一个新项目(p=0)是否在项目集合内,不考虑展望符
						if (!idx) {//需新增项目
							assign_item(&is->it[++is->l], &pd[j]);
							idx = is->l;
						}
						/* 展望符号推导：当前项目it为[A->α·Bβ,a],有产生式B->·γ,则新项目为[B->·γ,first(βa)] */
						string_set new_pros;
						for (int k = it->p + 2; k <= it->v.l+1; k++) {
							if (k == it->v.l + 1) {
								merge(&new_pros, &it->pros);
								break;
							}
							merge(&new_pros, &first[it->v.s[k]]);
							if (!in_set(&first[it->v.s[k]], nu))break;
						}/*判断项目展望符长度是否增加以判断闭包是否得到增加,如果是一个新项目,那么展望符长度必定增加(因为初始化为0)*/
						if (is->it[idx].pros.l < new_pros.l)flag = 1;
						merge(&is->it[idx].pros, &new_pros);
					}
				}

			}
		}
	}
}
/* 项目集s0输入字符x到新的项目集s1(不包括闭包计算) */
void Grammer::infer(item_set *s0, int x,item_set *s1) {
	s1->l = 0;
	for (int i = 1; i <= s0->l; i++) {
		item* it = &s0->it[i];
		if (it->p < it->v.l && x == it->v.s[it->p + 1]) {
			s1->l++;
			assign_item(&s1->it[s1->l], &s0->it[i]);
			s1->it[s1->l].p++;
		}
	}
}
/* 项目集是否在当前的项目集集合(it_set)中,若是返回index,该函数防止在构造dfa时重复定义项目集即状态 */
int Grammer::find_item_set(item_set* is) {
	for (int i = 1; i <= tot_s; i++) {
		item_set* t = &it_set[i];
		/* 判断项目集is是否和项目集t相等,等价于两者项目数量相等且is的项目均在t内 */
		if (t->l != is->l)continue;
		bool flag = 1;
		for (int j = 1; j <= is->l; j++) {
			if (!find_item(t, &is->it[j], 1)) { flag = 0; break; }
		}if (flag) return i;//项目集it_set[i]和项目集is相等

	}return 0;
}
/* 根据活前缀dfa构造LR(1)分析表
	具体做法:
	1.遍历项目规范集/状态中的所有项目,如果是归约项目(活前缀指针在末尾),在LR1表对应展望符的位置添加归约项,即-pd_index。
	且对于归约项目左侧为开始符号S(自己定义的),将LR1表空字符#下置为ACC(宏定义)。
	2.查看状态转换函数f,在对应字符下添加下一状态,注意不需要区分V、T(即ACTION和GOTO本质上一样),且不需要考虑输入空字符nu和开始字符S
*/
void Grammer::init_LR1_table() {
	for (int i = 1; i <= tot_s; i++) {
		item_set* is = &it_set[i];
		for (int j = 1; j <= is->l; j++) {
			item* it = &is->it[j];
			if (it->p == it->v.l) {//归约项目
				for (int k = 1; k <= it->pros.l; k++) {
					LR1[i][it->pros.s[k]] = -(it->who);//归约-pd_index,即-产生式下标
				}
				if (it->u == S)LR1[i][nu] = ACC;//ACC,分析表出口
			}
		}
		for (int j = 1; j < tot; j++) {
			if (j == nu)continue;
			if (!LR1[i][j])LR1[i][j] = f[i][j];
		}
	}
	/* 打印LR1分析表 */
	cout <<setfill('-')<< setiosflags(ios::right) << setw(50) << "LR1_TABLE" << setfill('-')<< setw(50) <<""<< endl;
	cout << setfill(' ')<<setiosflags(ios::right) << setw(2 * n + 3) << "ACTION";
	cout << setiosflags(ios::left) << setw(2 * n - 3)<<"";
	cout << "|";
	cout << setiosflags(ios::right) << setw(2 * (m - 1) + 2) << "GOTO";
	cout << setiosflags(ios::left) << setw(2 * (m-1) - 2) << "";
	cout << endl;
	for (int j = 1; j <= n; j++)cout << setiosflags(ios::left) << setw(4) << h_inv[j];
	cout << "|";
	for(int j=1;j<m;j++)cout << setiosflags(ios::left) << setw(4) << h_inv[j+n];
	cout << endl;
	for (int i = 1; i <= tot_s; i++) {
		for (int j = 1; j <= n; j++) {
			if (LR1[i][j] > 0) {
				if(LR1[i][j]==ACC)cout << setiosflags(ios::left) << setw(4) <<"ACC";
				else cout << setiosflags(ios::left) << setw(4) << LR1[i][j];
			}
			else if (!LR1[i][j])cout << setiosflags(ios::left) << setw(4) << "";
			else cout << setiosflags(ios::left) << setw(4) << string("r") + to_string(-LR1[i][j]);
		}cout << "|";
		for (int j = n + 1; j < tot; j++) {
			if (LR1[i][j])cout << setiosflags(ios::left) << setw(4) << string("s") + to_string(LR1[i][j]);
			else cout << setiosflags(ios::left) << setw(4) << "";
		}cout << endl;
	}
}
/* 构造活前缀dfa即规范项目集合包括状态转换函数f的构造 */
void Grammer::init_dfa() {
	/* 初始化dfa开始状态s1为开始产生式的闭包 */
	++tot_s;
	it_set[1].l = 1;
	assign_item(&it_set[1].it[1], &pd[1]);
	it_set[1].it[1].pros.s[++it_set[1].it[1].pros.l] = nu;
	closure(&it_set[1]);
	/* bfs得到活前缀dfa */
	queue<int>q;
	q.push(1);//push起始状态
	book[1] = 1;//标记
	while (!q.empty()) {
		int s0_index = q.front();
		item_set* s0 = &it_set[s0_index];
		q.pop();
		/* 枚举输入字符x包括V和T,不包括开始符S和空字符nu */
		for (int i = 1; i < tot; i++) {//tot是开始符不用考虑
			item_set *tmp=&it_set[tot_s+1];//这里很巧妙,以it_set最后一个项目集的后一个作为预选的新项目集,减少复杂度
			if (i == nu)continue;//nu
			infer(s0, i, tmp);//infer
			if (!tmp->l)continue;//输入字符i不能得到下一个状态
			closure(tmp);//计算闭包
			int idx = find_item_set(tmp);//判断是否已经有重复的dfa状态
			if (!idx)idx = ++tot_s;
			f[s0_index][i] = idx;
			if (!book[idx]) {//进入搜索队列
				book[idx] = 1;
				q.push(idx);
			}
			if (DEBUG) {
				cout << "状态s" << s0_index << endl;
				print_item_set(s0);
				cout << "输入symbol:" << h_inv[i] << "进入状态" << idx << endl;
				print_item_set(&it_set[idx]);
			}
		}
	}
}
/* 根据活前缀dfa以及状态转换函数即邻接表f生成可视化的graphiviz代码 */
void Grammer::generate_graphiviz_code(string filename) {
	ofstream ofs(filename);
	ofs << "digraph{\n";
	for (int i = 1; i <= tot_s; i++) {
		string is;//item set项目集的字符串形式
		ofs << "s" << i << "[label=\"" << string("state ") + to_string(i) + string("\n");
		print_item_set(&it_set[i], &ofs);
		ofs<< "\",shape=box];\n";
	}
	for (int i = 1; i <= tot_s; i++) {
		for (int j = 1; j <tot; j++) {
			if (j!=nu&&f[i][j]>0) {
				ofs <<"s"<< i << "->" <<"s"<< f[i][j]<<"[label=\""<<h_inv[j]<<"\"];\n";
			}
		}
	}
	ofs << "}";
}
/* 第将pd_index个产生式转为string形式 */
string Grammer::pd_string(int pd_index) {
	string x = "";
	x += h_inv[pd[pd_index].u] + string("->");
	for (int i = 1; i <= pd[pd_index].v.l; i++)x += h_inv[pd[pd_index].v.s[i]];
	return x;
}
/* 语法分析类构造函数 */
Grammer_Analysis::Grammer_Analysis(string filename) {
	top = 0;
	memset(state_stack, 0, sizeof(state_stack));
	memset(symbol_stack, 0, sizeof(symbol_stack));
	G.init(filename);
}
/* 状态栈、符号栈push */
void Grammer_Analysis::push(int state, int symbol) {
	state_stack[++top] = state;
	symbol_stack[top] = symbol;
}
/* 状态栈、符号栈get_top */
void Grammer_Analysis::get_top(int &state, int &symbol) {
	state=state_stack[top];
	symbol= symbol_stack[top];
}
/* 状态栈、符号栈pop */
void Grammer_Analysis::pop() {
	top-=top>0;
}