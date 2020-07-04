#include<vector>
#include<string>
#include<map>
#include<set>
#include<memory>   //动态内存管理 智能指针
#include<fstream>	//文件流 输入
#include<sstream>	//字符串流 用来提取每一行或每一个单词
#include<cctype>	//消除特殊符号
#include<iostream>
using namespace std;

//其实应该把QureyResult和TextQurey等类放在头文件中
class QueryResult;
class TextQuery{
	typedef vector<string>::size_type line_no; //行号类型为size_type using是咋写来着
public:
	//ifstream!!不是fstream 
	TextQuery(ifstream &in);
	//query方法 返回一个QueryResult类型的结果
	QueryResult query(const string &sought)const; //!const别漏！
	void display_map(); //调试输出函数
	//返回查询单词所在行集合的首迭代器 即找出单词所在的第一行 相当于set.begin()
	set<line_no>::iterator begin(const string &sought){ return wm[sought]->begin(); }  //wm[sought]是一个指向set的shared_ptr
	set<line_no>::iterator end(const string &sought){ return wm[sought]->end(); }
	shared_ptr<vector<string>> getfile(){ return file; }
private:
	shared_ptr<vector<string>> file;
	map<string, shared_ptr<set<line_no>>> wm;  //wordmap
};
//处理字符串的标点符号 忽略大小写 
string cleanup_str(const string &word){
	string ret; //空串
	for (auto it = word.begin(); it != word.end(); ++it){
		if (!ispunct(*it)) //忽略符号
			ret += tolower(*it); //一律转换为小写
	}
	return ret;  //记得返回的是一个新的对象
}
//构造函数 保存文件 建立映射
TextQuery::TextQuery(ifstream &in) :file(new vector<string>){
	string text;
	while (getline(in, text)){ //从文件in中取出每一行 赋给text
		file->push_back(text);
		line_no n = file->size() - 1;//行号等于vector的长度-1 如有一行文字 则是第0行
		//!是istringstream 不是stringstream！ 
		istringstream line(text); //将一行text赋值给字符串流line 这个字符串流可以包含空格 
		string word;
		//对于一行文本中的所有单词 将其单词和所在行号放入到map映射中
		while (line >> word){  //从字符串流line中获取每个字符串
			word = cleanup_str(word);
			auto &p = wm[word]; //p是一个指向set<line_no>的shared_ptr 记得设为引用 因为要插入行号会改变wm 同时改变指向它的指针p
			if (!p){
				p.reset(new set<line_no>); //若指针为空 意味着是第一次遇到该单词 
				//新分配一个set并令指针指向它
			}
			p->insert(n);  //将目前的行号加入该单词所在行的集合set<line_no>  p->insert代表set.insert()
		}
	}
}
//用来保存查询的结果 信息包括查询的文本 文件本身 以及查询文本所在行数的集合 同时能够进行输出
class QueryResult{
	friend ostream& print(ostream &os, const QueryResult &qr); //第二个参数是const 漏掉了 
	//！打印是只读 所以设置为const！
	using line_no = vector<string>::size_type;
public:
	//这里传递的string不是const& 为啥呢？
	QueryResult(string s, shared_ptr<vector<string>> f, shared_ptr<set<line_no>> l) :
		sought(s), file(f), lines(l){}
private:
	string sought; //查询单词
	shared_ptr<vector<string>> file; //保存的文本 用于输出整行
	shared_ptr<set<line_no>> lines; //sought对应的行号集合
};
//只有定义了QueryResult才能定义TextQuery中的query函数 因为返回类型是QueryResult
QueryResult TextQuery::query(const string &sought) const {
	static shared_ptr<set<line_no>> nodata(new set<line_no>); //如果没查找到 返回一个空的set的智能指针
	//auto lines = wm[sought]; !错误用法 此处不能用这个方法 
	//因为是查找，如果不存在的不能使用下标，否则会将其插入了
	auto loc = wm.find(sought); //find以关键字在map中进行查找 返回的是一个map迭代器 first指向string second指向set的共享指针
	if (loc == wm.end()){
		return QueryResult(sought, file, nodata);
	}
	else return QueryResult(sought, file, loc->second);  //这里有问题吗？为啥记得是->second？
}
//print的返回类型是形参ostream &os
ostream& print(ostream &os, const QueryResult &qr){
	os << "\""<< qr.sought << "\"" << " ocurrs " << qr.lines->size() << " time";
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
	TextQuery q(in);  //使用文本文件流构造一个TextQuery对象 也就是保存文件 建立映射那些工作
	while (true){
		cout << "enter word to look for, or q to quit: " << endl;
		string s;
		if (!(cin >> s) || s == "q") break;
		s = cleanup_str(s);
		QueryResult res = q.query(s);
		print(cout, res) << endl;
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
	ifstream textFile("1.txt");
	/*TextQuery test(in);
	test.display_map();*/
	RunQuery(textFile);
	
	return 0;
}