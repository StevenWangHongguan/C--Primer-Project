#include<iostream>
#include"my_String.h"
#include<vector>
#include<string>
using namespace std;
int main(){
	//vector<String> vec;
	//char *ch("char pointer");
	//String s(ch); //字符指针构造
	//String s1 = s; //拷贝构造
	//String s2(std::move(s)); //移动构造还是字符指针构造？
	//String s3;
	//s3 = "move ="; //移动赋值
	//String s1("123");
	//String s2("123");
	/*if (s1 < s2) cout << "<" << endl;
	if (s1 <= s2) cout << "<=" << endl;
	if (s1 > s2) cout << ">" << endl;
	if (s1 >= s2)cout << ">=" << endl;*/
	//cout << s1[2] << endl;
	String s("123");
	cout << s << endl;
	cin >> s;
	cout << s << endl;
	
	return 0;

}