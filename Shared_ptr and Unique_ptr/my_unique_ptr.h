#ifndef MY_UP_H
#define MY_UP_H
#include<iostream>
using namespace std;

template<typename T>
class UP{
public:
	UP() :p(nullptr){}
	explicit UP(T* up) :p(up){} //参数里没有const 
	UP(const &UP<T>) = delete; //拷贝构造函数定义为删除
	UP& operator=(const &UP<T>) = delete; //拷贝赋值运算符定义为删除
	~UP(){ if (p) delete p; }
	void reset(T *q = nullptr);
	T* release();
	T& operator*(){ return *p; }
	T& operator*() const { return *p; }
private:
	T* p;
};
template<typename T>
void UP<T>::reset(T *q = nullptr){
	if (p)
		delete p;
	p = q;
}
template<typename T>
T* UP<T>::release(){
	T* q = p; // 这里不能直接置p为空 要用另外的指针指向原有的对象
	p = nullptr;
	return q;  //返回原来的指针
}
#endif