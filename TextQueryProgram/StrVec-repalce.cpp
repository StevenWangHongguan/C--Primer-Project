#include<vector>
#include<string>
#include<map>
#include<set>
#include<memory>
#include<fstream>
#include<sstream>
#include<cctype>
#include<iostream>

using namespace std;

class QueryResult;
class TextQuery{
	typedef vector<string>::size_type line_no; //using是咋写来着
public:
	//ifstream!!不是fstream 
	TextQuery(ifstream &in);
	QueryResult query(const string &sought)const; //!const别漏！
	void display_map(); //调试输出辅助函数
	set<line_no>::iterator begin(const string &sought){ return wm[sought]->begin(); }
	set<line_no>::iterator end(const string &sought){ return wm[sought]->end(); }
	shared_ptr<vector<string>> getfile(){ return file; }
private:
	shared_ptr<vector<string>> file;
	map<string, shared_ptr<set<line_no>>> wm;
};
//处理字符串的标点符号 忽略大小写
string cleanup_str(const string &word){
	string ret; //空串
	for (auto it = word.begin(); it != word.end(); ++it){
		if (!ispunct(*it)) //忽略符号
			ret += tolower(*it); //一律转换为小写
	}
	return ret;
}
//构造函数 保存文件 建立映射
TextQuery::TextQuery(ifstream &in) :file(new StrVec){
	string text;
	while (getline(in, text)){
		file->push_back(text);
		line_no n = file->size() - 1;//行号等于vector的长度-1 如有一行文字 则是第0行
		//!是istringstream 不是stringstream！ 
		istringstream line(text);
		string word;
		while (line >> word){
			word = cleanup_str(word);
			auto &p = wm[word]; //p是一个指向set<line_no>的shared_ptr 记得设为引用 因为要插入set会改变指针
			if (!p){
				p.reset(new set<line_no>); //若指针为空 意味着是第一次遇到该单词 
				//新分配一个set并令指针指向它
			}
			p->insert(n);
		}
	}
}

class QueryResult{
	friend ostream& print(ostream &os, const QueryResult &qr); //第二个参数是const 漏掉了 
	//！打印是只读 所以设置为const！
	using line_no = vector<string>::size_type;
public:
	//这里传递的string既不是const 也不是引用 为啥呢？
	QueryResult(string s, shared_ptr<vector<string>> f, shared_ptr<set<line_no>> l) :
		sought(s), file(f), lines(l){}
private:
	string sought;
	shared_ptr<vector<string>> file;
	shared_ptr<set<line_no>> lines;
};
QueryResult TextQuery::query(const string &sought) const {
	static shared_ptr<set<line_no>> nodata(new set<line_no>);
	//auto lines = wm[sought]; !错误用法 此处不能用这个方法 
	//因为是查找，如果不存在的不能使用下标，否则会将其插入了
	auto loc = wm.find(sought);
	if (loc == wm.end()){
		return QueryResult(sought, file, nodata);
	}
	else return QueryResult(sought, file, loc->second);  //这里有问题吗？为啥记得是->second？
}
//print的返回类型是形参ostream &os
ostream& print(ostream &os, const QueryResult &qr){
	os << qr.sought << " ocurrs " << qr.lines->size() << " time";
	if (qr.lines->size() > 1) os << "s";
	os << endl;
	for (auto num : *(qr.lines)){  //在set<line_no>中 每行输出
		os << "\t" << "(line " << num + 1 << ") " << *(qr.file->begin() + num) << endl;
		//qr.file是一个指向shared_ptr的指针 ->begin()取得vector的首迭代器
	}
	return os;
}
void RunQuery(ifstream &in){
	if (!in){
		cerr << "can not open file" << endl;
		return;
	}
	TextQuery q(in);
	while (true){
		cout << "enter word to look for, or q to quit: " << endl;
		string s;
		if (!(cin >> s) || s == "q") break;
		s = cleanup_str(s);
		print(cout, q.query(s)) << endl;
	}
}
void TextQuery::display_map(){
	for (auto mapiter = wm.begin(); mapiter != wm.end(); ++mapiter){
		cout << "word:" << mapiter->first << "; lines:" << "{";
		auto lines = mapiter->second; //lines是一个指向set<line_no>的shared_ptr
		auto lineiter = lines->begin();
		while (lineiter != lines->end()){
			cout << *lineiter+1;
			if (++lineiter != lines->end())
				cout << ",";
		}
		cout << "}" << endl;
	}
}
int main(){
	ifstream in("1.txt");
	RunQuery(in);
	//TextQuery test(in);
	//test.display_map();
	return 0;
}