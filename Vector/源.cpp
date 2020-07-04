#include<iostream>
#include<algorithm>
#include"myVec.h"
#include<vector>
#include<string>
using namespace std;
template<typename T>
void print(Vec<T> v){
	for (auto x : v)
		cout << x << " ";
	cout << endl;
}
int main(){
	Vec<string> v1 = { "hello","world" };
	cout << "v1:" << endl;
	print(v1);
	cout << "push_back !"<< endl;
	v1.push_back("!");
	print(v1);
	cout << "pop_back v1" << endl;
	v1.pop_back();
	print(v1);
	
	return 0;
}