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
	typedef vector<string>::size_type line_no; //using��զд����
public:
	//ifstream!!����fstream 
	TextQuery(ifstream &in);
	QueryResult query(const string &sought)const; //!const��©��
	void display_map(); //���������������
	set<line_no>::iterator begin(const string &sought){ return wm[sought]->begin(); }
	set<line_no>::iterator end(const string &sought){ return wm[sought]->end(); }
	shared_ptr<vector<string>> getfile(){ return file; }
private:
	shared_ptr<vector<string>> file;
	map<string, shared_ptr<set<line_no>>> wm;
};
//�����ַ����ı����� ���Դ�Сд
string cleanup_str(const string &word){
	string ret; //�մ�
	for (auto it = word.begin(); it != word.end(); ++it){
		if (!ispunct(*it)) //���Է���
			ret += tolower(*it); //һ��ת��ΪСд
	}
	return ret;
}
//���캯�� �����ļ� ����ӳ��
TextQuery::TextQuery(ifstream &in) :file(new StrVec){
	string text;
	while (getline(in, text)){
		file->push_back(text);
		line_no n = file->size() - 1;//�кŵ���vector�ĳ���-1 ����һ������ ���ǵ�0��
		//!��istringstream ����stringstream�� 
		istringstream line(text);
		string word;
		while (line >> word){
			word = cleanup_str(word);
			auto &p = wm[word]; //p��һ��ָ��set<line_no>��shared_ptr �ǵ���Ϊ���� ��ΪҪ����set��ı�ָ��
			if (!p){
				p.reset(new set<line_no>); //��ָ��Ϊ�� ��ζ���ǵ�һ�������õ��� 
				//�·���һ��set����ָ��ָ����
			}
			p->insert(n);
		}
	}
}

class QueryResult{
	friend ostream& print(ostream &os, const QueryResult &qr); //�ڶ���������const ©���� 
	//����ӡ��ֻ�� ��������Ϊconst��
	using line_no = vector<string>::size_type;
public:
	//���ﴫ�ݵ�string�Ȳ���const Ҳ�������� Ϊɶ�أ�
	QueryResult(string s, shared_ptr<vector<string>> f, shared_ptr<set<line_no>> l) :
		sought(s), file(f), lines(l){}
private:
	string sought;
	shared_ptr<vector<string>> file;
	shared_ptr<set<line_no>> lines;
};
QueryResult TextQuery::query(const string &sought) const {
	static shared_ptr<set<line_no>> nodata(new set<line_no>);
	//auto lines = wm[sought]; !�����÷� �˴�������������� 
	//��Ϊ�ǲ��ң���������ڵĲ���ʹ���±꣬����Ὣ�������
	auto loc = wm.find(sought);
	if (loc == wm.end()){
		return QueryResult(sought, file, nodata);
	}
	else return QueryResult(sought, file, loc->second);  //������������Ϊɶ�ǵ���->second��
}
//print�ķ����������β�ostream &os
ostream& print(ostream &os, const QueryResult &qr){
	os << qr.sought << " ocurrs " << qr.lines->size() << " time";
	if (qr.lines->size() > 1) os << "s";
	os << endl;
	for (auto num : *(qr.lines)){  //��set<line_no>�� ÿ�����
		os << "\t" << "(line " << num + 1 << ") " << *(qr.file->begin() + num) << endl;
		//qr.file��һ��ָ��shared_ptr��ָ�� ->begin()ȡ��vector���׵�����
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
		auto lines = mapiter->second; //lines��һ��ָ��set<line_no>��shared_ptr
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