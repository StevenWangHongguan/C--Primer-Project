#ifndef MY_SP_H
#define MY_SP_H
#include<iostream>
#include<utility> //forward方法
using namespace std;

template<typename T>
class SP{
public:
	SP() :p(nullptr), use(nullptr){} //默认构造 
	//使用对象指针直接初始化 类似SP s_p（new string()） 记得声明为explicit
	explicit SP(T *pt) :p(pt), use(new size_t(1)){}  //参数里没有const！！
	SP(const SP<T> &sp) :p(sp.p), use(p.use) { if (use) ++*use; }//拷贝构造函数
	SP& operator=(const SP<T> &sp); //拷贝赋值运算符
	~SP(); //析构函数
	T& operator*(){ return *p; } //解引用普通版本
	T& operator*()const{ return *p; } //常量版本 前面的const需要吗？ 书上不需要哦
private:
	T *p; // 指向元素对象
	size_t *use; //指向计数器
};

template<typename T>
SP<T>& SP<T>::operator=(const SP<T> &rhs){
	if (rhs.use)	//拷贝赋值运算 右边对象计数器+1 左边计数器-1
		++*rhs.use; //因为传来的对象也可能是空的（默认构造）所以需要判断一下use
	if (use&&--*use == 0){ //左边对象计数器指针不为空 且赋值之后减少为0 执行析构
		delete p;
		delete use;
	}
	p = rhs.p;
	use = rhs.use;
	return *this;
}
template<typename T>
SP<T>::~SP(){
	if (use&&--*use == 0){ //如果use指针不为空 且计数器-1后变为0 销毁对象、释放空间
		delete p;
		delete use;
	}
}
// make_SP函数可以接受最多10个参数，然后把它们传递给类型T的构造函数，创建一个shared_ptr<T>的对象并返回。
template<typename T, class...Args>// Args...模板参数包 表示此模板中会有多个Class类型参数
SP<T> make_SP(Args&&...args)//右值引用类型 保持参数原有类型信息； ...args拓展Args
{
	return SP<T>(new T(std::forward<Args>args...));
}
// 例如 SP<string> sp = make_SP<string>("hello world")； ("hello world")会传递给string的构造函数
// SP<vector<int>> vcp = make_SP<vector<int>>(10,2); (10,2)会传递给int类型的vector构造函数 
#endif