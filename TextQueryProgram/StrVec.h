#ifndef MY_STRVEC_H
#define MY_STRVEC_H
#include<string>
#include<memory>
#include<utility>  //pair�����ڸ�ͷ�ļ���
#include<initializer_list>
#include<iostream>
using namespace std;
//Ҫ�ڹ��̵������е�Ԥ�����������м��� _SCL_SECURE_NO_WARNINGS �����޷�ʹ��uninitialized_copy
class StrVec{
public:
	//���캯�����������Ƴ�Ա
	// StrVec(); //�����費��Ҫ����Ĭ�ϵĹ��캯��������ֱ�����б��ʼ���ķ�ʽ 
	// ����Ҫ ��Ϊ�������������Ĺ��캯��һ�� �޲��� ������ͬһ������ ����Ĭ�ϵĹ��캯��
	StrVec() :elements(nullptr), first_free(nullptr), cap(nullptr){}
	StrVec(initializer_list<string> il);
	StrVec(const StrVec&);
	StrVec& operator=(const StrVec&);
	~StrVec(){ free(); }
	//�ӿڹ��ܺ��� const��©��
	size_t size() const { return first_free - elements; }
	size_t capacity() const { return cap - elements; }
	string* begin() const { return elements; }
	string* end() const { return first_free; }
	void push_back(const string&);
	//reverse���Ըı����������� ��ֻ�����䲻�ܼ���
	void reserve(size_t n){ if (n > capacity()) reallocate(n); }
	//resize���Ըı������е���Ŀ �����ܸı�����������
	void resize(size_t n);  //������ֵ�汾
	void resize(size_t n, const string &s); //����ֵ�汾
private:
	//��������ڴ�ռ�ľ�̬���� �˱����;����ű�StrVec���ڴ�ռ�
	//����������ط�VS�������� Ҫȡ����̬���� 
	allocator<string> alloc;
	//�������졢������ֵʹ�� ����һ��ռ䲢����һ�����е�StrVec�е�Ԫ�ع�ȥ
	pair<string*, string*> alloc_n_copy(const string*, const string*); //�����е�const©���� 
	//���ٶ��� �ͷſռ� ������������
	void free();
	//���ռ��Ƿ���� ����Ҫ�����ٷ��� ������StrVec������Ԫ��ʹ��
	void check_n_alloc(){ if (size() == capacity()) reallocate(); }
	//��StrVec�ռ�������� ÿ���ڴ�ռ䲻��ʱ������������������ �����ƶ�����Ա���µ��ڴ�ռ�
	void reallocate();
	void reallocate(size_t n);
	//˽�����ݳ�Ա
	string *elements;
	string *first_free;
	string *cap;
};
//����������˽�еĳ�Ա��������
pair<string*, string*>
StrVec::alloc_n_copy(const string *b, const string *e){
	auto newdata = alloc.allocate(e - b); //����ͬ����С��һ���ڴ�ռ�
	//!!!�˴���uninitialized_copy ����û��n!!!!!!!!
	return make_pair(newdata, uninitialized_copy(b, e, newdata)); //�˴���make_pair ����pair
	//�����ǲ���{}����ʽ���� ��˼Ӧ����Ҳ������һ��pair
}
void StrVec::free(){ //�˴�����϶� 
	//���ܴ���deallcateһ����ָ�� ��elmentsΪ�� ʲôҲ����
	if (elements){
		/*while (first_free != elements){        //������Ӧ��Ҳ���� ��Ϊ���ϵ������ǲ��ı�
		alloc.destroy(--first_free);		 //first_free ��Ϊfree vector֮�� �ڴ�ռ����ͷ�
		}*/										 //��Щָ�붼��ɿ���ָ��
		for (auto p = first_free; p != elements;)
			alloc.destroy(--p);
		alloc.deallocate(elements, cap - elements); //�ͷ�����vector�ռ�
	}
}
void StrVec::reallocate(){
	auto newcapacity = size() ? size() * 2 : 1; //��Ŀǰ������Ϊ0 ����Ϊ���� �������Ϊ1���ڴ�ռ�
	auto newdata = alloc.allocate(newcapacity);
	auto elem = elements; //ָ����������һ��Ԫ��
	auto dest = newdata; //ָ�����������һ��Ԫ��
	for (size_t i = 0; i != size(); ++i){ //�ƶ�size()��Ԫ�أ��Σ�
		//!�ؼ���� ��destָ��Ŀռ乹����� �������ƶ�������һ��string����
		alloc.construct(dest++, std::move(*elem++));
	}
	//�������˴�����һ��free() һ�������ƶ���Ԫ�ؾ��ͷž��ڴ�ռ� 
	free();
	//�������ݳ�Ա
	elements = newdata;
	first_free = dest;
	cap = elements + newcapacity; //�˾��ô�� �Ե��ϸ�
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
//�����ǿ��ƿ�����Ա����
StrVec::StrVec(initializer_list<string> il){
	auto newdata = alloc_n_copy(il.begin(), il.end());
	elements = newdata.first;
	first_free = cap = newdata.second;
}
StrVec::StrVec(const StrVec &rhs){ //�������캯��
	auto newdata = alloc_n_copy(rhs.begin(), rhs.end()); //����һ���µĿռ䲢��lhs���󿽱�����
	//��alloc_n_copy��ʵ����ֻ������lhs����Ч���֣�������ڴ�ռ��ϵĿ�Ԫ��û�п������� ��Ϊ��end-begin
	//������ô˵ �·���ռ䲢����������StrVec�����ص� �޶���ռ��˷ѵ�
	elements = newdata.first;
	first_free = cap = newdata.second;
}
//��ֵ����Ҫ���������������Ĺ��� ��Ҫ�ܴ����Ը�ֵ�����
StrVec& StrVec::operator=(const StrVec &rhs){
	auto newdata = alloc_n_copy(rhs.begin(), rhs.end());
	free(); //���پɵĶ����ͷžɵĿռ�
	elements = newdata.first;
	first_free = cap = newdata.second;
	return *this; //©���� �ǵø�ֵ����������ķ����������������
}
//���ܺ��� ���Ԫ��
void StrVec::push_back(const string& s){
	check_n_alloc();
	// *first_free++ = s; ����Ҫ����construct�ķ�����allocator�Ϲ���
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