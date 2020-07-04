#include<vector>
#include<string>
#include<map>
#include<set>
#include<memory>   //��̬�ڴ���� ����ָ��
#include<fstream>	//�ļ��� ����
#include<sstream>	//�ַ����� ������ȡÿһ�л�ÿһ������
#include<cctype>	//�����������
#include<iostream>
using namespace std;

//��ʵӦ�ð�QureyResult��TextQurey�������ͷ�ļ���
class QueryResult;
class TextQuery{
	typedef vector<string>::size_type line_no; //�к�����Ϊsize_type using��զд����
public:
	//ifstream!!����fstream 
	TextQuery(ifstream &in);
	//query���� ����һ��QueryResult���͵Ľ��
	QueryResult query(const string &sought)const; //!const��©��
	void display_map(); //�����������
	//���ز�ѯ���������м��ϵ��׵����� ���ҳ��������ڵĵ�һ�� �൱��set.begin()
	set<line_no>::iterator begin(const string &sought){ return wm[sought]->begin(); }  //wm[sought]��һ��ָ��set��shared_ptr
	set<line_no>::iterator end(const string &sought){ return wm[sought]->end(); }
	shared_ptr<vector<string>> getfile(){ return file; }
private:
	shared_ptr<vector<string>> file;
	map<string, shared_ptr<set<line_no>>> wm;  //wordmap
};
//�����ַ����ı����� ���Դ�Сд 
string cleanup_str(const string &word){
	string ret; //�մ�
	for (auto it = word.begin(); it != word.end(); ++it){
		if (!ispunct(*it)) //���Է���
			ret += tolower(*it); //һ��ת��ΪСд
	}
	return ret;  //�ǵ÷��ص���һ���µĶ���
}
//���캯�� �����ļ� ����ӳ��
TextQuery::TextQuery(ifstream &in) :file(new vector<string>){
	string text;
	while (getline(in, text)){ //���ļ�in��ȡ��ÿһ�� ����text
		file->push_back(text);
		line_no n = file->size() - 1;//�кŵ���vector�ĳ���-1 ����һ������ ���ǵ�0��
		//!��istringstream ����stringstream�� 
		istringstream line(text); //��һ��text��ֵ���ַ�����line ����ַ��������԰����ո� 
		string word;
		//����һ���ı��е����е��� ���䵥�ʺ������кŷ��뵽mapӳ����
		while (line >> word){  //���ַ�����line�л�ȡÿ���ַ���
			word = cleanup_str(word);
			auto &p = wm[word]; //p��һ��ָ��set<line_no>��shared_ptr �ǵ���Ϊ���� ��ΪҪ�����кŻ�ı�wm ͬʱ�ı�ָ������ָ��p
			if (!p){
				p.reset(new set<line_no>); //��ָ��Ϊ�� ��ζ���ǵ�һ�������õ��� 
				//�·���һ��set����ָ��ָ����
			}
			p->insert(n);  //��Ŀǰ���кż���õ��������еļ���set<line_no>  p->insert����set.insert()
		}
	}
}
//���������ѯ�Ľ�� ��Ϣ������ѯ���ı� �ļ����� �Լ���ѯ�ı����������ļ��� ͬʱ�ܹ��������
class QueryResult{
	friend ostream& print(ostream &os, const QueryResult &qr); //�ڶ���������const ©���� 
	//����ӡ��ֻ�� ��������Ϊconst��
	using line_no = vector<string>::size_type;
public:
	//���ﴫ�ݵ�string����const& Ϊɶ�أ�
	QueryResult(string s, shared_ptr<vector<string>> f, shared_ptr<set<line_no>> l) :
		sought(s), file(f), lines(l){}
private:
	string sought; //��ѯ����
	shared_ptr<vector<string>> file; //������ı� �����������
	shared_ptr<set<line_no>> lines; //sought��Ӧ���кż���
};
//ֻ�ж�����QueryResult���ܶ���TextQuery�е�query���� ��Ϊ����������QueryResult
QueryResult TextQuery::query(const string &sought) const {
	static shared_ptr<set<line_no>> nodata(new set<line_no>); //���û���ҵ� ����һ���յ�set������ָ��
	//auto lines = wm[sought]; !�����÷� �˴�������������� 
	//��Ϊ�ǲ��ң���������ڵĲ���ʹ���±꣬����Ὣ�������
	auto loc = wm.find(sought); //find�Թؼ�����map�н��в��� ���ص���һ��map������ firstָ��string secondָ��set�Ĺ���ָ��
	if (loc == wm.end()){
		return QueryResult(sought, file, nodata);
	}
	else return QueryResult(sought, file, loc->second);  //������������Ϊɶ�ǵ���->second��
}
//print�ķ����������β�ostream &os
ostream& print(ostream &os, const QueryResult &qr){
	os << "\""<< qr.sought << "\"" << " ocurrs " << qr.lines->size() << " time";
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
	TextQuery q(in);  //ʹ���ı��ļ�������һ��TextQuery���� Ҳ���Ǳ����ļ� ����ӳ����Щ����
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
	ifstream textFile("1.txt");
	/*TextQuery test(in);
	test.display_map();*/
	RunQuery(textFile);
	
	return 0;
}