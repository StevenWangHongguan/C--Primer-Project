#ifndef MY_STRING_H
#define MY_STRIGN_H
#include<iostream>
#include<algorithm>
#include<memory>
#include<utility>
#include<initializer_list>
using namespace std;

class String{
	friend ostream& operator<<(ostream &os, const String &s);
	friend istream& operator>>(istream &is, String &s);
	friend bool operator==(const String &lhs, const String &rhs);
	friend bool operator!=(const String &lhs, const String &rhs);
	friend bool operator<(const String &lhs, const String &rhs);
	friend bool operator<=(const String &lhs, const String &rhs);
	friend bool operator>(const String &lhs, const String &rhs);
	friend bool operator>=(const String &lhs, const String &rhs);
public:
	String() :element(nullptr), first_free(nullptr){ } //默认构造函数
	String(const char*); //C风格字符串构造
	String(const String&); //拷贝构造
	String(String&&) _NOEXCEPT; //移动构造 注意右值引用 没有const
	String& operator=(const String&); //赋值运算符
	String& operator=(String&&) _NOEXCEPT;//移动赋值运算符 没有const
	~String(){ free(); } //析构函数
	size_t size() const { return first_free - element; }
	char* begin() const { return element; }
	char* end() const { return first_free; }
	char& operator[](size_t n){ return element[n]; }
	const char& operator[](size_t n) const { return element[n]; }
private:
	allocator<char> alloc;
	pair<char*, char*> alloc_n_copy(const char*,const char*);
	void free();
	//由于string没有push_back的功能 不需要reallocate和check 也不需要cap指针
	char *element;
	char *first_free;
};
pair<char*, char*> String::alloc_n_copy(const char *b,const char *e){
	auto newdata = alloc.allocate(e - b);
	return{ newdata, uninitialized_copy(b, e, newdata) };
}
void String::free(){
	//若首指针为空 代表string为空 不用执行任何事
	if (element){
		auto p = first_free;
		while (p != element)
			alloc.destroy(--p);
		alloc.deallocate(element, first_free - element);
	}
	//for_each配合lambda表达式 对该范围内的每个成员（指针指向成员类型为char) 取得该对象指针并根据指针销毁
	/*for_each(element, first_free, [this](char &c) { alloc.destroy(&c); });*/
}
//C风格字符串构造函数
String::String(const char*pc){
	//!!!关键 要转换成非常量的指针 因为传递过来是一个字面值常量的话 
	//它其实是一个const char*指针 要转换成非常量char* 才能向后移动
	auto end = const_cast<char*>(pc); 
	while (*end != '\0') ++end; //找到尾后指针 它指向'\0'
	auto res = alloc_n_copy(pc, end); //分配一块内存 并将元素复制到上面
	element = res.first;
	first_free = res.second;
}
//拷贝控制成员
//拷贝构造 代表是初始化的时候拷贝构造 刚开始没有分配空间的 所以不需要free
String::String(const String &rhs){
	auto newdata = alloc_n_copy(rhs.element, rhs.first_free);
	element = newdata.first;
	first_free = newdata.second;
}
//移动构造  
String::String(String &&s)_NOEXCEPT
{
	element = s.element;
	first_free = s.first_free;
	s.first_free = s.element = nullptr;
}
//而拷贝赋值运算符 代表左边的运算对象已经初始化过了 有分配的内存空间 现在要将右边的对象赋值给它 
//所以原来的空间要释放掉
//拷贝赋值
String& String::operator=(const String &rhs){
	auto newdata = alloc_n_copy(rhs.element, rhs.first_free); //新开辟一块内存 将源String暂存 
	free(); //析构当前String
	element = newdata.first;
	first_free = newdata.second;
	return *this;
}
//移动赋值
String& String::operator=(String &&s) _NOEXCEPT
{
	if (*this != s){ //二者不是同一个对象 若同一个对象不需要操作 此处应该是*this！=s吧？ 
		free();
		element = s.element;
		first_free = s.first_free;
		s.first_free = s.element = nullptr;
	}
	return *this;
}
//重载运算符
ostream& operator<<(ostream &os, const String &s){
	auto p = s.element;
	while (p != s.first_free)
		os << *p++;
	return os;
}
istream& operator>>(istream &is, String &s){
	s.free(); //要对str进行输入 则先将其free()
	size_t capacity = 1; //string的容量 初始为1
	s.element=s.first_free=s.alloc.allocate(capacity);
	char c;
	while (c=is.get()){
		if (c == '\n' || c == ' ') break;
		//  容量不足 扩容
		if(s.size() >= capacity){ //目前数量等于容量 再输入一个char就装不下了
			capacity *= 2; //扩充两倍容量 并移动
			auto oldelem = s.element; //存储旧string的首元素
			auto oldfirst_free = s.first_free;//记录旧元素的尾后元素 用于后续释放空间
			auto dest = s.element; //旧string首元素 用于移动
			size_t oldsize = s.size(); //存储旧string元素数量
			s.element = s.first_free = s.alloc.allocate(capacity); //分配新的空间
			for (size_t i = 0; i != oldsize; ++i){ //将旧元素移动到新的空间中
				s.alloc.construct(s.first_free++, std::move(*dest++));
			}
			s.alloc.deallocate(oldelem, oldfirst_free - oldelem);//释放旧的空间
		}
		//在末尾构造一个新元素
		s.alloc.construct(s.first_free++, c);
	}
	return is;
}
bool operator==(const String &lhs, const String &rhs){
	if (lhs.size()==rhs.size()){  //记得size()函数要声明成const 否则const对象无法调用
		auto p1 = lhs.element;
		auto p2 = rhs.element;
		for (int i = 0; i != lhs.size(); ++i){
			if (*p1 != *p2) return false;
		}
		return true;
	}
	else return false;
}
bool operator!=(const String &lhs, const String &rhs){
	return !(lhs == rhs);
}
bool operator<(const String &lhs, const String &rhs){
	auto p1 = lhs.begin(), p2 = rhs.begin();
	for (; p1 != lhs.end() && p2 != rhs.end(); ++p1, ++p2){
		if (*p1 < *p2) return true; //lhs第一个字符就小于 返回true
		else if (*p1 > *p2) return false; //反之 返回false
	}
	//到达此处 说明前面比较的字符串都相等 且至少其中一个到达了字符串末尾
	if (p1 == lhs.end()&&p2!=rhs.end())  //p1先到达末尾(且p2没到)说明lhs的长度小于rhs 正确
		return true;
	else
		return false; //否则 即（>=） <比较也是false的
}
bool operator<=(const String &lhs, const String &rhs){
	return (lhs < rhs) || (lhs == rhs);
}
bool operator>(const String &lhs, const String &rhs){
	return !(lhs <= rhs);
}
bool operator>=(const String &lhs, const String &rhs){
	return !(lhs < rhs);
}
#endif
