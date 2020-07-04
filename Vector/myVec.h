#ifndef MY_VEC_H
#define MY_VEC_H
#include<memory>
#include<utility>  
#include<initializer_list>
#include<iostream>
#include<algorithm>
using namespace std;
//要在工程的属性中的预处理器定义中加上 _SCL_SECURE_NO_WARNINGS 否则无法使用uninitialized_copy
template<typename T> class Vec;
//将运算符声明为模板 这些声明是Vec中的友元函数的参数（即<T>）声明所需要的
template<typename T> bool operator == (const Vec<T> &lhs, const Vec<T> &rhs);
template<typename T> bool operator != (const Vec<T> &lhs, const Vec<T> &rhs);
template<typename T> bool operator < (const Vec<T> &lhs, const Vec<T> &rhs);
template<typename T> bool operator <= (const Vec<T> &lhs, const Vec<T> &rhs);
template<typename T> bool operator > (const Vec<T> &lhs, const Vec<T> &rhs);
template<typename T> bool operator >= (const Vec<T> &lhs, const Vec<T> &rhs);

template<typename T>
class Vec{
	//友元声明 说明使用Vec模板中的模板形参T来作为它们自己的模板形参 是一对一的友好关系
	//!注意 在友元声明中 重载运算符的后面 需要有模板实参<T> 但是在类外的函数定义时不能有这个实参
	//（猜测） 这个<T>应该是要和class Vec建立一对一的友好关系 即实现相同类型的实例化的运算符
	friend bool operator== <T> (const Vec<T> &lhs, const Vec<T> &rhs); //比如Vec的类型是int 那么就会与operator==<int>建立友好关系 才能使用此==运算符比较int类型的Vec对象
	friend bool operator!= <T> (const Vec<T> &lhs, const Vec<T> &rhs);
	friend bool operator< <T> (const Vec<T> &lhs, const Vec<T> &rhs);
	friend bool operator<= <T> (const Vec<T> &lhs, const Vec<T> &rhs);
	friend bool operator> <T> (const Vec<T> &lhs, const Vec<T> &rhs);
	friend bool operator>= <T> (const Vec<T> &lhs, const Vec<T> &rhs);
public:
	//在类模板自己的作用域中 可以直接使用模板名而不提供实参（即<T>） 但是参数需要提供实参（好像不需要..)
	Vec() :elements(nullptr), first_free(nullptr), cap(nullptr){}
	Vec(initializer_list<T> il);  //可变参数表来构造
	Vec(const Vec<T>&); //拷贝构造函数    Vec中的<T>好像可以省略
	Vec(Vec<T> &&s) _NOEXCEPT; //移动构造函数
	Vec& operator=(const Vec<T>&); //拷贝赋值运算符
	Vec& operator=(Vec<T> &&rhs) _NOEXCEPT;//移动赋值运算符 记住右值没有const
	Vec& operator=(initializer_list<T> il); //花括号列表的方式赋值
	~Vec(){ free(); }  //析构函数
	//下标运算符
	T& operator[](size_t n){ return elements[n]; } 
	const T& operator[](size_t n)const{ return elements[n]; }
	//功能函数 const别漏掉
	size_t size() const { return first_free - elements; }
	size_t capacity() const { return cap - elements; }
	T* begin() const { return elements; }
	T* end() const { return first_free; }
	void push_back(const T&); //拷贝版本
	void push_back(T &&); //移动版本
	void pop_back(); //取出尾元素
	//reverse可以改变容器的容量 且只能扩充不能减少
	void reserve(size_t n){ if (n > capacity()) reallocate(n); }
	//resize可以改变容器中的数目 但不能改变容器的容量
	void resize(size_t n);  //不带初值版本
	void resize(size_t n, const T &s); //带初值版本
private:
	//负责管理内存空间的（静态）变量 这个地方VS编译会出错 要取消静态声明 
	allocator<T> alloc;
	//拷贝构造、拷贝赋值使用 分配一块空间并复制一个已有的Vec中的元素过去
	pair<T*, T*> alloc_n_copy(const T*, const T*); //参数中的const漏掉了 
	//销毁对象 释放空间 用于析构函数
	void free();
	//检查空间是否充足 若不足进行再分配 用于向Vec中增加元素使用
	void check_n_alloc(){ if (size() == capacity()) reallocate(); }
	//对Vec空间进行扩容 每次内存空间不够时扩容现有容量的两倍 并“移动”成员到新的内存空间
	void reallocate();
	void reallocate(size_t n);
	//私有数据成员
	T *elements;
	T *first_free;
	T *cap;
};
//在类的外部定义成员函数 需要加上模板声明
template<typename T>   //此函数是一个类模板中的成员函数
pair<T*, T*>           //返回类型
Vec<T>::alloc_n_copy(const T *b, const T *e){ //声明类的作用域：：记得需要实参<T>
	auto newdata = alloc.allocate(e - b); //分配同样大小的一块内存空间
	//uninitialized_copy 应该是将b-e中指向的数据复制到以newdata为起始地址的空间中 并返回尾指针
	return make_pair(newdata, uninitialized_copy(b, e, newdata)); //此处是make_pair 不是pair
}

template<typename T>
void Vec<T>::free(){  
	if (elements){
	#pragma region 使用说明 
		/*while (first_free != elements){        //这样做应该也可以 因为书上的做法是不改变
		alloc.destroy(--first_free);		 //first_free 因为free vector之后 内存空间已释放
		}*/										 //这些指针都变成空悬指针
	#pragma endregion
		for (auto p = first_free; p != elements;)
			alloc.destroy(--p);
		alloc.deallocate(elements, cap - elements); //释放整个vector空间
	}
	#pragma region for_each和lambda表达式
	//采用for_each()和Lambda表达方式较好 但是只有alloc是静态变量才能如此使用
	//void Vec<T>::free(){
	//	for_each(elements, first_free, [](T &s){ alloc.destroy(&s); }); 
	//}
	#pragma endregion
}

template<typename T>
void Vec<T>::reallocate(){
	auto newcapacity = size() ? size() * 2 : 1; //若目前容量不为0 分配为两倍 否则分配为1个内存空间
	auto newdata = alloc.allocate(newcapacity);
	auto elem = elements; //指向旧数组的下一个元素
	auto dest = newdata; //指向新数组的下一个元素
	for (size_t i = 0; i != size(); ++i){ //移动size()个元素（次）
		//!关键语句 在dest指向的空间构造对象 并且是移动构造了一个T对象
		alloc.construct(dest++, std::move(*elem++));
	}
	//！！！此处少了一个free() 一旦我们移动完元素就释放旧内存空间 
	free();
	//重置数据成员
	elements = newdata;
	first_free = dest;
	cap = elements + newcapacity; //此句对么？ 对的老哥
}
template<typename T>
void Vec<T>::reallocate(size_t n){
	auto newdata = alloc.allocate(n);
	auto elem = elements;
	auto dest = newdata;
	for (size_t i = 0; i != size(); i++)
		alloc.construct(dest++, std::move(*elem++)); //后续添了个++
	free();
	elements = newdata;
	first_free = dest;
	cap = newdata + newcapacity;
}
//以下是拷贝控制操作定义
template<typename T>
Vec<T>::Vec(initializer_list<T> il){ //可变参数列表构造函数
	auto newdata = alloc_n_copy(il.begin(), il.end());
	elements = newdata.first;
	first_free = cap = newdata.second;
}
template<typename T>
Vec<T>::Vec(const Vec<T> &rhs){ //拷贝构造函数
	auto newdata = alloc_n_copy(rhs.begin(), rhs.end()); //分配一块新的空间并将lhs对象拷贝过来
	elements = newdata.first;
	first_free = cap = newdata.second;
}
template<typename T>	
Vec<T>::Vec(Vec<T> &&s) _NOEXCEPT : //移动构造函数
	elements(s.elements), first_free(s.first_free), cap(s.cap)
{	
	s.elements = s.first_free = s.cap = nullptr;
}
template<typename T>
Vec<T>& Vec<T>::operator=(const Vec<T> &rhs){ //拷贝赋值运算符
	auto newdata = alloc_n_copy(rhs.begin(), rhs.end());
	free(); //销毁旧的对象并释放旧的空间 和拷贝构造函数的唯一区别...
	elements = newdata.first;
	first_free = cap = newdata.second;
	return *this; 
}
template<typename T>
Vec<T>& Vec<T>::operator=(Vec<T> &&rhs) _NOEXCEPT //移动赋值运算符
{
	if (this != &rhs){ //这里是个引用 是指针
		free();  //自身要析构
		elements = rhs.elements;
		first_free = rhs.first_free;
		cap = rhs.cap;
		rhs.elements = rhs.first_free = rhs.cap = nullptr; //源移动对象也要置空
	}
	return *this;
}
template<typename T>
Vec<T>& Vec<T>::operator=(initializer_list<T> il){  //可变参数列表赋值
	auto data = alloc_n_copy(il.begin(), il.end());
	free();  //和可变参数列表构造函数的唯一区别... 当然还有返回值
	elements = data.first;
	first_free = cap = data.second;
	return *this;
}
//功能函数 
template<typename T>
void Vec<T>::push_back(const T& s){
	check_n_alloc();
	alloc.construct(first_free++, s);
}
template<typename T>
void Vec<T>::push_back(T&& s){
	check_n_alloc();
	alloc.construct(first_free++, std::move(s));
}
template<typename T>
void Vec<T>::pop_back(){
	if (first_free!=elements){ //若尾元素不为空
		alloc.destroy(--first_free);
	}
	else throw out_of_range("pop_back on empty Vec");
}
template<typename T>
void Vec<T>::resize(size_t n){
	if (n > size()){
		while (size() < n){
			this->push_back("");
		}
	}
	else if (n < size()){
		while (n < size()){
			alloc.destroy(--first_free);
		}
	}
}
template<typename T>
void Vec<T>::resize(size_t n, const T& s){
	if (n > size()){
		while (size() < n){
			this->push_back(s);
		}
	}
	else if (n < size()){
		while (n < size()){
			alloc.destroy(--first_free);
		}
	}
}
//重载关系运算符
template<typename T>
bool operator==(const Vec<T> &lhs, const Vec<T> &rhs){
	if (lhs.size() == rhs.size()){ //长度相等才有可能相等
		auto p1 = lhs.begin(), p2 = rhs.begin();
		for (; p1 != lhs.end(); ++p1, ++p2)
			if (*p1 != *p2) return false; //元素不等返回false
		return true;
	}
	else return false;
}
template<typename T>
bool operator!=(const Vec<T> &lhs, const Vec<T> &rhs){
	return !(lhs == rhs);
}
template<typename T>
bool operator<(const Vec<T> &lhs, const Vec<T> &rhs){
	auto p1 = lhs.begin(), p2 = rhs.begin();
	for (; p1 != lhs.end() && p2 != rhs.end(); ++p1, ++p2){
		if (*p1 < *p2) return true; //lhs第一个字符串就小于 返回true
		else if (*p1 > *p2) return false; //反之 返回false
	}
	//到达此处 说明前面比较的字符串都相等 且至少其中一个到达了字符串末尾
	if (p1 == lhs.end() && p2 != rhs.end())  //p1先到达末尾(且p2没到)说明lhs的长度小于rhs 正确
		return true;
	else
		return false; //否则 即（>=） <比较也是false的
}
template<typename T>
bool operator<=(const Vec<T> &lhs, const Vec<T> &rhs){
	return (lhs < rhs) || (lhs == rhs);
}
template<typename T>
bool operator>(const Vec<T> &lhs, const Vec<T> &rhs){
	return !(lhs <= rhs);
}
template<typename T>
bool operator>=(const Vec<T> &lhs, const Vec<T> &rhs){
	return !(lhs < rhs);
}
#endif