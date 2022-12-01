#include"Grammer_Analysis.h"
/* ��x��ӵ�set�� */
void add(string_set *set, int x) {
	for (int i = 1; i <= set->l; i++) {
		if (set->s[i] == x)return ;
	}set->s[++set->l] = x;
}
/* �ж�x�Ƿ��ڼ����� */
bool in_set(string_set *set, int x) {
	for (int i = 1; i <= set->l; i++) {
		if (set->s[i] == x)return true;
	}
	return false;
}
/* ��y�ϲ���x */
void merge(string_set* x, string_set *y) {
	for (int i = 1; i <= y->l; i++) {
		if (in_set(x, y->s[i]))continue;
		x->s[++x->l] = y->s[i];
	}
}
/* ��ӡ���ż� */
void print_set(string_set *x,string sep,map<int,string>h) {
	for (int i = 1; i <= x->l; i++) {
		cout << h[x->s[i]];
		if (i < x->l)cout << sep;
	}
	cout << endl;
}
/* ��ĸ�ֵassign���� */
void assign_item(item* x, item* y) {
	x->u = y->u;
	x->p = y->p;
	x->who = y->who;
	x->v.l = y->v.l;
	x->pros.l = y->pros.l;
	for (int i = 1; i <= y->v.l; i++)x->v.s[i] = y->v.s[i];
	for (int i = 1; i <= y->pros.l; i++)x->pros.s[i] = y->pros.s[i];
}
/* ���s����һ���ս��/���ս��,sΪ���򷵻ؿ� */
string Grammer::nxt(string& s) {
	if (s == "\0")return "\0";
	string t;
	int i = 0;
	if (i + 1 < s.length() && s[i] == '-' && s[i + 1] == '>') { i += 2; }//->
	for (int j = i; j < s.length(); j++) {
		t = s.substr(i, j - i + 1);
		if (h[t]) {//���ս��/���ս��
			s = j == s.length() - 1 ? "\0" : s.substr(j + 1, s.length() - j - 1);
			return t;
		}
	}
}
/* �ķ����캯�� */
Grammer::Grammer() {
	pd_num = 0; tot = 0; tot_s = 0;
	memset(f, 0, sizeof(f)); 
	memset(book, 0, sizeof(book));
	memset(LR1, 0, sizeof(LR1));
}
/* �����ķ�����ʼ����������->int��ӳ�䡢����ʽ�Ķ����Լ�����init_first_set��init_dfa�� */
void Grammer::init(string filename) {
	ifs.open(filename);
	if (!ifs.is_open()) {
		cout << "�ķ��ļ���ʧ��";
		return;
	}
	string tmp;
	ifs >> tmp;
	ifs >> n;
	string s;
	for (int i = 1; i <= n; i++) {//�����ս��
		ifs >> s;
		h[s] = ++tot;
		h_inv[tot] = s;
	}if (!h["#"]) {
		h["#"] = ++tot;//���ַ�ҲҪ����
		h_inv[tot] = "#";
		++n;
	}nu = h["#"];
	ifs >> m;
	for (int i = 1; i <= m; i++) {//������ս��
		ifs >> s;
		h[s] = ++tot;
		h_inv[tot] = s;
	}h["S"] = ++tot; h_inv[tot] = "S"; S = h["S"]; ++m;
	/* ��������ʽ,"S"->S,�õ��ع��ķ�G',��֤G'��Լʱ����� */
	pd_num++;
	pd[pd_num].u = h["S"]; 
	pd[pd_num].v.s[++pd[pd_num].v.l] = h[tmp];
	pd[pd_num].who = pd_num;
	/* �������ʽ */
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
	/* ��ӡ��������ӳ���ϵ */
	cout << setfill('-') << setiosflags(ios::right) << setw(50) << "map" << setfill('-') << setw(50) << "" << endl;
	cout.flags(ios::left);
	for (int i = 1; i <= tot; i++) {
		cout <<setfill(' ')<< setw(4) << h_inv[i] << h[h_inv[i]] << endl;
	}
	/* ��ӡ����ʽ */
	cout << setfill('-') << setiosflags(ios::right) << setw(50) << "PRODUCTION" << setfill('-') << setw(50) << "" << endl;
	for (int i = 1; i <= pd_num; i++) {
		cout <<"r"<< i<<" "<<h_inv[pd[i].u] << "->";
		for (int j = 1; j <= pd[i].v.l; j++)
			cout << h_inv[pd[i].v.s[j]];
		cout << endl;
	}
	init_first_set();//����first��
	init_follow_set();//����follow��
	init_dfa();//�����ǰ׺dfa��״̬ת������f
	init_LR1_table();//����LR1������
	generate_graphiviz_code("gph_code.txt");//����dfaͼ��graphiviz����
}
/* �����ķ���follow����
* ���Ƚ����ַ�#���뿪ʼ����S��follow����
* �����������ʽA->��B��,��first(��)/{��}����B��follow����
* ����A->B1B2...Bn
* ��Bn��Ϊnu��A��follow������Bn��;��Bi+1...Bn��first���������ַ�nu,��A��follow������Bi��
* ֱ������V��follow���ϲ�����
*/
void Grammer::init_follow_set() {
	add(&follow[S], nu);//���ַ�
	/* �����������ʽA->��B��,��first(��)/{��}����B��follow���� */
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
	/* A->B,��A��follow������B */
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
	}/* ���follow�� */
	cout << setfill('-') << setiosflags(ios::right) << setw(50) << "Follow" << setfill('-') << setw(50) << "" << endl;
	for (int i = 1; i <= tot; i++) {
		cout << "follow[" << h_inv[i] << "]=";
		print_set(&follow[i], ",", h_inv);
	}
	
}
/* �����ķ���first����
����A->B1B2...Bn����ʽ(Bi����Ϊ�ս�����߷��ս��)��
1.��B1��first���ϼӵ�A��;
2.���B1...Bi-1��first��������#(���ַ�),��Bi��first���ϼ���A��first����
�ظ�ִ��ֱ���ķ�first���ϱ���
*/
void Grammer::init_first_set() {
	for (int i = 1; i <= n; i++)add(&first[i], i);
	bool flag = 1;//flag=1��ʾfirst��δ����
	while (flag) {
		flag = 0;
		for (int i = 1; i <= pd_num; i++) {
			for (int j = 1; j <= pd[i].v.l; j++) {
				int pre_l = first[pd[i].u].l;
				merge(&first[pd[i].u], &first[pd[i].v.s[j]]);
				flag |= first[pd[i].u].l > pre_l;//Ԫ������
				if (!in_set(&first[pd[i].v.s[j]], nu))break;
			}
		}
	}
	/* ���first�� */
	cout << setfill('-') << setiosflags(ios::right) << setw(50) << "FIRST" << setfill('-') << setw(50) << "" << endl;
	for (int i = 1; i <= tot; i++) {
		cout << "first[" << h_inv[i] << "]=";
		print_set(&first[i], ",",h_inv);
	}
}
/* �ж��Ƿ��ս�ʦ�� */
bool Grammer::is_V(int x) { return x > n; }
/* �ж����ս�� */
bool Grammer::is_T(int x) { return x <= n; }
/* �ж���Ŀit�Ƿ�����Ŀ��is��,flag=0������չ����,flag=1�����ж�չ����,�ؼ���who��pҪ���,��������������ʽ�Լ���ǰ׺ָ��λ�� */
int Grammer::find_item(item_set *is, item *it,int flag) {
	for (int i = 1; i <= is->l; i++) {
		item *tmp = &is->it[i];
		if (tmp->p == it->p && tmp->who == it->who) {
			if(!flag)return i;
			/* �ж�չ�����Ƿ���ȫ��� */
			bool fflag = 1;
			if (tmp->pros.l != it->pros.l)continue;
			for (int j = 1; j <= it->pros.l; j++) {
				if (!in_set(&tmp->pros, it->pros.s[j])) { fflag = 0; break; }
			}if (fflag)return i;
		}

	}return 0;
}
/* debug�����Ŀ�� */
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
				if (ofs == NULL)cout << "��";
				else *ofs << "��";
			}
		}
		if (ofs == NULL)cout << "��";
		else *ofs << "��";
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
/* ������Ŀ���հ�,����:
��Ŀ��һ����ĿitΪ[A->����B��,a],�в���ʽB->����,���������ĿΪ[B->����,first(��a)],ֱ������
ע��:first(ABC)����first��������
*/
void Grammer::closure(item_set* is) {
	bool flag = 1;//flag=1��ʾ��Ŀ��δ����
	while (flag) {
		flag = 0;
		for (int i = 1; i <= is->l; i++) {
			item *it = &is->it[i];
			int u;
			if (it->p < it->v.l && is_V(u = it->v.s[it->p + 1])) {//A->����u��
				for (int j = 1; j <= pd_num; j++) {
					if (pd[j].u == u) {//u->����
						int idx = find_item(is, &pd[j],0);//�ж�һ������Ŀ(p=0)�Ƿ�����Ŀ������,������չ����
						if (!idx) {//��������Ŀ
							assign_item(&is->it[++is->l], &pd[j]);
							idx = is->l;
						}
						/* չ�������Ƶ�����ǰ��ĿitΪ[A->����B��,a],�в���ʽB->����,������ĿΪ[B->����,first(��a)] */
						string_set new_pros;
						for (int k = it->p + 2; k <= it->v.l+1; k++) {
							if (k == it->v.l + 1) {
								merge(&new_pros, &it->pros);
								break;
							}
							merge(&new_pros, &first[it->v.s[k]]);
							if (!in_set(&first[it->v.s[k]], nu))break;
						}/*�ж���Ŀչ���������Ƿ��������жϱհ��Ƿ�õ�����,�����һ������Ŀ,��ôչ�������ȱض�����(��Ϊ��ʼ��Ϊ0)*/
						if (is->it[idx].pros.l < new_pros.l)flag = 1;
						merge(&is->it[idx].pros, &new_pros);
					}
				}

			}
		}
	}
}
/* ��Ŀ��s0�����ַ�x���µ���Ŀ��s1(�������հ�����) */
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
/* ��Ŀ���Ƿ��ڵ�ǰ����Ŀ������(it_set)��,���Ƿ���index,�ú�����ֹ�ڹ���dfaʱ�ظ�������Ŀ����״̬ */
int Grammer::find_item_set(item_set* is) {
	for (int i = 1; i <= tot_s; i++) {
		item_set* t = &it_set[i];
		/* �ж���Ŀ��is�Ƿ����Ŀ��t���,�ȼ���������Ŀ���������is����Ŀ����t�� */
		if (t->l != is->l)continue;
		bool flag = 1;
		for (int j = 1; j <= is->l; j++) {
			if (!find_item(t, &is->it[j], 1)) { flag = 0; break; }
		}if (flag) return i;//��Ŀ��it_set[i]����Ŀ��is���

	}return 0;
}
/* ���ݻ�ǰ׺dfa����LR(1)������
	��������:
	1.������Ŀ�淶��/״̬�е�������Ŀ,����ǹ�Լ��Ŀ(��ǰ׺ָ����ĩβ),��LR1���Ӧչ������λ����ӹ�Լ��,��-pd_index��
	�Ҷ��ڹ�Լ��Ŀ���Ϊ��ʼ����S(�Լ������),��LR1����ַ�#����ΪACC(�궨��)��
	2.�鿴״̬ת������f,�ڶ�Ӧ�ַ��������һ״̬,ע�ⲻ��Ҫ����V��T(��ACTION��GOTO������һ��),�Ҳ���Ҫ����������ַ�nu�Ϳ�ʼ�ַ�S
*/
void Grammer::init_LR1_table() {
	for (int i = 1; i <= tot_s; i++) {
		item_set* is = &it_set[i];
		for (int j = 1; j <= is->l; j++) {
			item* it = &is->it[j];
			if (it->p == it->v.l) {//��Լ��Ŀ
				for (int k = 1; k <= it->pros.l; k++) {
					LR1[i][it->pros.s[k]] = -(it->who);//��Լ-pd_index,��-����ʽ�±�
				}
				if (it->u == S)LR1[i][nu] = ACC;//ACC,���������
			}
		}
		for (int j = 1; j < tot; j++) {
			if (j == nu)continue;
			if (!LR1[i][j])LR1[i][j] = f[i][j];
		}
	}
	/* ��ӡLR1������ */
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
/* �����ǰ׺dfa���淶��Ŀ���ϰ���״̬ת������f�Ĺ��� */
void Grammer::init_dfa() {
	/* ��ʼ��dfa��ʼ״̬s1Ϊ��ʼ����ʽ�ıհ� */
	++tot_s;
	it_set[1].l = 1;
	assign_item(&it_set[1].it[1], &pd[1]);
	it_set[1].it[1].pros.s[++it_set[1].it[1].pros.l] = nu;
	closure(&it_set[1]);
	/* bfs�õ���ǰ׺dfa */
	queue<int>q;
	q.push(1);//push��ʼ״̬
	book[1] = 1;//���
	while (!q.empty()) {
		int s0_index = q.front();
		item_set* s0 = &it_set[s0_index];
		q.pop();
		/* ö�������ַ�x����V��T,��������ʼ��S�Ϳ��ַ�nu */
		for (int i = 1; i < tot; i++) {//tot�ǿ�ʼ�����ÿ���
			item_set *tmp=&it_set[tot_s+1];//���������,��it_set���һ����Ŀ���ĺ�һ����ΪԤѡ������Ŀ��,���ٸ��Ӷ�
			if (i == nu)continue;//nu
			infer(s0, i, tmp);//infer
			if (!tmp->l)continue;//�����ַ�i���ܵõ���һ��״̬
			closure(tmp);//����հ�
			int idx = find_item_set(tmp);//�ж��Ƿ��Ѿ����ظ���dfa״̬
			if (!idx)idx = ++tot_s;
			f[s0_index][i] = idx;
			if (!book[idx]) {//������������
				book[idx] = 1;
				q.push(idx);
			}
			if (DEBUG) {
				cout << "״̬s" << s0_index << endl;
				print_item_set(s0);
				cout << "����symbol:" << h_inv[i] << "����״̬" << idx << endl;
				print_item_set(&it_set[idx]);
			}
		}
	}
}
/* ���ݻ�ǰ׺dfa�Լ�״̬ת���������ڽӱ�f���ɿ��ӻ���graphiviz���� */
void Grammer::generate_graphiviz_code(string filename) {
	ofstream ofs(filename);
	ofs << "digraph{\n";
	for (int i = 1; i <= tot_s; i++) {
		string is;//item set��Ŀ�����ַ�����ʽ
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
/* �ڽ�pd_index������ʽתΪstring��ʽ */
string Grammer::pd_string(int pd_index) {
	string x = "";
	x += h_inv[pd[pd_index].u] + string("->");
	for (int i = 1; i <= pd[pd_index].v.l; i++)x += h_inv[pd[pd_index].v.s[i]];
	return x;
}
/* �﷨�����๹�캯�� */
Grammer_Analysis::Grammer_Analysis(string filename) {
	top = 0;
	memset(state_stack, 0, sizeof(state_stack));
	memset(symbol_stack, 0, sizeof(symbol_stack));
	G.init(filename);
}
/* ״̬ջ������ջpush */
void Grammer_Analysis::push(int state, int symbol) {
	state_stack[++top] = state;
	symbol_stack[top] = symbol;
}
/* ״̬ջ������ջget_top */
void Grammer_Analysis::get_top(int &state, int &symbol) {
	state=state_stack[top];
	symbol= symbol_stack[top];
}
/* ״̬ջ������ջpop */
void Grammer_Analysis::pop() {
	top-=top>0;
}