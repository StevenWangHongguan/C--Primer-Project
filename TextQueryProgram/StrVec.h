#ifndef MY_STRVEC_H
#define MY_STRVEC_H
#include<string>
#include<memory>
#include<utility>  //pair定义在该头文件中
#include<initializer_list>
#include<iostream>
using namespace std;
//要在工程的属性中的预处理器定义中加上 _SCL_SECURE_NO_WARNINGS 否则无法使用uninitialized_copy
class StrVec{
public:
	//构造函数、拷贝控制成员
	// StrVec(); //到底需不需要声明默认的构造函数？还是直接用列表初始化的方式 
	// 不需要 因为这个函数跟下面的构造函数一样 无参数 所以是同一个函数 且是默认的构造函数
	StrVec() :elements(nullptr), first_free(nullptr), cap(nullptr){}
	StrVec(initializer_list<string> il);
	StrVec(const StrVec&);
	StrVec& operator=(const StrVec&);
	~StrVec(){ free(); }
	//接口功能函数 const别漏掉
	size_t size() const { return first_free - elements; }
	size_t capacity() const { return cap - elements; }
	string* begin() const { return elements; }
	string* end() const { return first_free; }
	void push_back(const string&);
	//reverse可以改变容器的容量 且只能扩充不能减少
	void reserve(size_t n){ if (n > capacity()) reallocate(n); }
	//resize可以改变容器中的数目 但不能改变容器的容量
	void resize(size_t n);  //不带初值版本
	void resize(size_t n, const string &s); //带初值版本
private:
	//负责管理内存空间的静态变量 此变量就决定着本StrVec的内存空间
	//！！！这个地方VS编译会出错 要取消静态声明 
	allocator<string> alloc;
	//拷贝构造、拷贝赋值使用 分配一块空间并复制一个已有的StrVec中的元素过去
	pair<string*, string*> alloc_n_copy(const string*, const string*); //参数中的const漏掉了 
	//销毁对象 释放空间 用于析构函数
	void free();
	//检查空间是否充足 若需要进行再分配 用于向StrVec中增加元素使用
	void check_n_alloc(){ if (size() == capacity()) reallocate(); }
	//对StrVec空间进行扩容 每次内存空间不够时扩容现有容量的两倍 并“移动”成员到新的内存空间
	void reallocate();
	void reallocate(size_t n);
	//私有数据成员
	string *elements;
	string *first_free;
	string *cap;
};
//以下三个是私有的成员函数定义
pair<string*, string*>
StrVec::alloc_n_copy(const string *b, const string *e){
	auto newdata = alloc.allocate(e - b); //分配同样大小的一块内存空间
	//!!!此处是uninitialized_copy 后面没有n!!!!!!!!
	return make_pair(newdata, uninitialized_copy(b, e, newdata)); //此处是make_pair 不是pair
	//书中是采用{}的形式返回 意思应该是也构造了一个pair
}
void StrVec::free(){ //此处错误较多 
	//不能传给deallcate一个空指针 若elments为空 什么也不做
	if (elements){
		/*while (first_free != elements){        //这样做应该也可以 因为书上的做法是不改变
		alloc.destroy(--first_free);		 //first_free 因为free vector之后 内存空间已释放
		}*/										 //这些指针都变成空悬指针
		for (auto p = first_free; p != elements;)
			alloc.destroy(--p);
		alloc.deallocate(elements, cap - elements); //释放整个vector空间
	}
}
void StrVec::reallocate(){
	auto newcapacity = size() ? size() * 2 : 1; //若目前容量不为0 分配为两倍 否则分配为1个内存空间
	auto newdata = alloc.allocate(newcapacity);
	auto elem = elements; //指向旧数组的下一个元素
	auto dest = newdata; //指向新数组的下一个元素
	for (size_t i = 0; i != size(); ++i){ //移动size()个元素（次）
		//!关键语句 在dest指向的空间构造对象 并且是移动构造了一个string对象
		alloc.construct(dest++, std::move(*elem++));
	}
	//！！！此处少了一个free() 一旦我们移动完元素就释放旧内存空间 
	free();
	//重置数据成员
	elements = newdata;
	first_free = dest;
	cap = elements + newcapacity; //此句对么？ 对的老哥
}
void StrVec::reallocate(size_t n){
	auto newcapacity = n;
	auto newdata = alloc.allocate(newcapacity);
	auto elem = elements;
	auto dest = newdata;
	for (size_t i = 0; i != size(); i++){
		alloc.construct(dest++, std::move(*elem));
		elements = newdata;
		first_free = dest;
		cap = newdata + newcapacity;
	}
}
//以下是控制拷贝成员定义
StrVec::StrVec(initializer_list<string> il){
	auto newdata = alloc_n_copy(il.begin(), il.end());
	elements = newdata.first;
	first_free = cap = newdata.second;
}
StrVec::StrVec(const StrVec &rhs){ //拷贝构造函数
	auto newdata = alloc_n_copy(rhs.begin(), rhs.end()); //分配一块新的空间并将lhs对象拷贝过来
	//在alloc_n_copy中实际上只拷贝了lhs的有效部分，多余的内存空间上的空元素没有拷贝过来 因为是end-begin
	//可以这么说 新分配空间并拷贝过来的StrVec是满载的 无多余空间浪费的
	elements = newdata.first;
	first_free = cap = newdata.second;
}
//赋值拷贝要包括拷贝和析构的功能 且要能处理自赋值的情况
StrVec& StrVec::operator=(const StrVec &rhs){
	auto newdata = alloc_n_copy(rhs.begin(), rhs.end());
	free(); //销毁旧的对象并释放旧的空间
	elements = newdata.first;
	first_free = cap = newdata.second;
	return *this; //漏掉了 记得赋值拷贝运算符的返回类型是类对象本身
}
//功能函数 添加元素
void StrVec::push_back(const string& s){
	check_n_alloc();
	// *first_free++ = s; 错误？要采用construct的方法在allocator上构造
	alloc.construct(first_free++, s);
}
void StrVec::resize(size_t n){
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
void StrVec::resize(size_t n, const string& s){
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
#endif