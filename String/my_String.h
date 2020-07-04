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
	String() :element(nullptr), first_free(nullptr){ } //Ĭ�Ϲ��캯��
	String(const char*); //C����ַ�������
	String(const String&); //��������
	String(String&&) _NOEXCEPT; //�ƶ����� ע����ֵ���� û��const
	String& operator=(const String&); //��ֵ�����
	String& operator=(String&&) _NOEXCEPT;//�ƶ���ֵ����� û��const
	~String(){ free(); } //��������
	size_t size() const { return first_free - element; }
	char* begin() const { return element; }
	char* end() const { return first_free; }
	char& operator[](size_t n){ return element[n]; }
	const char& operator[](size_t n) const { return element[n]; }
private:
	allocator<char> alloc;
	pair<char*, char*> alloc_n_copy(const char*,const char*);
	void free();
	//����stringû��push_back�Ĺ��� ����Ҫreallocate��check Ҳ����Ҫcapָ��
	char *element;
	char *first_free;
};
pair<char*, char*> String::alloc_n_copy(const char *b,const char *e){
	auto newdata = alloc.allocate(e - b);
	return{ newdata, uninitialized_copy(b, e, newdata) };
}
void String::free(){
	//����ָ��Ϊ�� ����stringΪ�� ����ִ���κ���
	if (element){
		auto p = first_free;
		while (p != element)
			alloc.destroy(--p);
		alloc.deallocate(element, first_free - element);
	}
	//for_each���lambda���ʽ �Ը÷�Χ�ڵ�ÿ����Ա��ָ��ָ���Ա����Ϊchar) ȡ�øö���ָ�벢����ָ������
	/*for_each(element, first_free, [this](char &c) { alloc.destroy(&c); });*/
}
//C����ַ������캯��
String::String(const char*pc){
	//!!!�ؼ� Ҫת���ɷǳ�����ָ�� ��Ϊ���ݹ�����һ������ֵ�����Ļ� 
	//����ʵ��һ��const char*ָ�� Ҫת���ɷǳ���char* ��������ƶ�
	auto end = const_cast<char*>(pc); 
	while (*end != '\0') ++end; //�ҵ�β��ָ�� ��ָ��'\0'
	auto res = alloc_n_copy(pc, end); //����һ���ڴ� ����Ԫ�ظ��Ƶ�����
	element = res.first;
	first_free = res.second;
}
//�������Ƴ�Ա
//�������� �����ǳ�ʼ����ʱ�򿽱����� �տ�ʼû�з���ռ�� ���Բ���Ҫfree
String::String(const String &rhs){
	auto newdata = alloc_n_copy(rhs.element, rhs.first_free);
	element = newdata.first;
	first_free = newdata.second;
}
//�ƶ�����  
String::String(String &&s)_NOEXCEPT
{
	element = s.element;
	first_free = s.first_free;
	s.first_free = s.element = nullptr;
}
//��������ֵ����� ������ߵ���������Ѿ���ʼ������ �з�����ڴ�ռ� ����Ҫ���ұߵĶ���ֵ���� 
//����ԭ���Ŀռ�Ҫ�ͷŵ�
//������ֵ
String& String::operator=(const String &rhs){
	auto newdata = alloc_n_copy(rhs.element, rhs.first_free); //�¿���һ���ڴ� ��ԴString�ݴ� 
	free(); //������ǰString
	element = newdata.first;
	first_free = newdata.second;
	return *this;
}
//�ƶ���ֵ
String& String::operator=(String &&s) _NOEXCEPT
{
	if (*this != s){ //���߲���ͬһ������ ��ͬһ��������Ҫ���� �˴�Ӧ����*this��=s�ɣ� 
		free();
		element = s.element;
		first_free = s.first_free;
		s.first_free = s.element = nullptr;
	}
	return *this;
}
//���������
ostream& operator<<(ostream &os, const String &s){
	auto p = s.element;
	while (p != s.first_free)
		os << *p++;
	return os;
}
istream& operator>>(istream &is, String &s){
	s.free(); //Ҫ��str�������� ���Ƚ���free()
	size_t capacity = 1; //string������ ��ʼΪ1
	s.element=s.first_free=s.alloc.allocate(capacity);
	char c;
	while (c=is.get()){
		if (c == '\n' || c == ' ') break;
		//  �������� ����
		if(s.size() >= capacity){ //Ŀǰ������������ ������һ��char��װ������
			capacity *= 2; //������������ ���ƶ�
			auto oldelem = s.element; //�洢��string����Ԫ��
			auto oldfirst_free = s.first_free;//��¼��Ԫ�ص�β��Ԫ�� ���ں����ͷſռ�
			auto dest = s.element; //��string��Ԫ�� �����ƶ�
			size_t oldsize = s.size(); //�洢��stringԪ������
			s.element = s.first_free = s.alloc.allocate(capacity); //�����µĿռ�
			for (size_t i = 0; i != oldsize; ++i){ //����Ԫ���ƶ����µĿռ���
				s.alloc.construct(s.first_free++, std::move(*dest++));
			}
			s.alloc.deallocate(oldelem, oldfirst_free - oldelem);//�ͷžɵĿռ�
		}
		//��ĩβ����һ����Ԫ��
		s.alloc.construct(s.first_free++, c);
	}
	return is;
}
bool operator==(const String &lhs, const String &rhs){
	if (lhs.size()==rhs.size()){  //�ǵ�size()����Ҫ������const ����const�����޷�����
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
		if (*p1 < *p2) return true; //lhs��һ���ַ���С�� ����true
		else if (*p1 > *p2) return false; //��֮ ����false
	}
	//����˴� ˵��ǰ��Ƚϵ��ַ�������� ����������һ���������ַ���ĩβ
	if (p1 == lhs.end()&&p2!=rhs.end())  //p1�ȵ���ĩβ(��p2û��)˵��lhs�ĳ���С��rhs ��ȷ
		return true;
	else
		return false; //���� ����>=�� <�Ƚ�Ҳ��false��
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
