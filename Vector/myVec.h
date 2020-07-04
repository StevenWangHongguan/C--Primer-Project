#ifndef MY_VEC_H
#define MY_VEC_H
#include<memory>
#include<utility>  
#include<initializer_list>
#include<iostream>
#include<algorithm>
using namespace std;
//Ҫ�ڹ��̵������е�Ԥ�����������м��� _SCL_SECURE_NO_WARNINGS �����޷�ʹ��uninitialized_copy
template<typename T> class Vec;
//�����������Ϊģ�� ��Щ������Vec�е���Ԫ�����Ĳ�������<T>����������Ҫ��
template<typename T> bool operator == (const Vec<T> &lhs, const Vec<T> &rhs);
template<typename T> bool operator != (const Vec<T> &lhs, const Vec<T> &rhs);
template<typename T> bool operator < (const Vec<T> &lhs, const Vec<T> &rhs);
template<typename T> bool operator <= (const Vec<T> &lhs, const Vec<T> &rhs);
template<typename T> bool operator > (const Vec<T> &lhs, const Vec<T> &rhs);
template<typename T> bool operator >= (const Vec<T> &lhs, const Vec<T> &rhs);

template<typename T>
class Vec{
	//��Ԫ���� ˵��ʹ��Vecģ���е�ģ���β�T����Ϊ�����Լ���ģ���β� ��һ��һ���Ѻù�ϵ
	//!ע�� ����Ԫ������ ����������ĺ��� ��Ҫ��ģ��ʵ��<T> ����������ĺ�������ʱ���������ʵ��
	//���²⣩ ���<T>Ӧ����Ҫ��class Vec����һ��һ���Ѻù�ϵ ��ʵ����ͬ���͵�ʵ�����������
	friend bool operator== <T> (const Vec<T> &lhs, const Vec<T> &rhs); //����Vec��������int ��ô�ͻ���operator==<int>�����Ѻù�ϵ ����ʹ�ô�==������Ƚ�int���͵�Vec����
	friend bool operator!= <T> (const Vec<T> &lhs, const Vec<T> &rhs);
	friend bool operator< <T> (const Vec<T> &lhs, const Vec<T> &rhs);
	friend bool operator<= <T> (const Vec<T> &lhs, const Vec<T> &rhs);
	friend bool operator> <T> (const Vec<T> &lhs, const Vec<T> &rhs);
	friend bool operator>= <T> (const Vec<T> &lhs, const Vec<T> &rhs);
public:
	//����ģ���Լ����������� ����ֱ��ʹ��ģ���������ṩʵ�Σ���<T>�� ���ǲ�����Ҫ�ṩʵ�Σ�������Ҫ..)
	Vec() :elements(nullptr), first_free(nullptr), cap(nullptr){}
	Vec(initializer_list<T> il);  //�ɱ������������
	Vec(const Vec<T>&); //�������캯��    Vec�е�<T>�������ʡ��
	Vec(Vec<T> &&s) _NOEXCEPT; //�ƶ����캯��
	Vec& operator=(const Vec<T>&); //������ֵ�����
	Vec& operator=(Vec<T> &&rhs) _NOEXCEPT;//�ƶ���ֵ����� ��ס��ֵû��const
	Vec& operator=(initializer_list<T> il); //�������б�ķ�ʽ��ֵ
	~Vec(){ free(); }  //��������
	//�±������
	T& operator[](size_t n){ return elements[n]; } 
	const T& operator[](size_t n)const{ return elements[n]; }
	//���ܺ��� const��©��
	size_t size() const { return first_free - elements; }
	size_t capacity() const { return cap - elements; }
	T* begin() const { return elements; }
	T* end() const { return first_free; }
	void push_back(const T&); //�����汾
	void push_back(T &&); //�ƶ��汾
	void pop_back(); //ȡ��βԪ��
	//reverse���Ըı����������� ��ֻ�����䲻�ܼ���
	void reserve(size_t n){ if (n > capacity()) reallocate(n); }
	//resize���Ըı������е���Ŀ �����ܸı�����������
	void resize(size_t n);  //������ֵ�汾
	void resize(size_t n, const T &s); //����ֵ�汾
private:
	//��������ڴ�ռ�ģ���̬������ ����ط�VS�������� Ҫȡ����̬���� 
	allocator<T> alloc;
	//�������졢������ֵʹ�� ����һ��ռ䲢����һ�����е�Vec�е�Ԫ�ع�ȥ
	pair<T*, T*> alloc_n_copy(const T*, const T*); //�����е�const©���� 
	//���ٶ��� �ͷſռ� ������������
	void free();
	//���ռ��Ƿ���� ����������ٷ��� ������Vec������Ԫ��ʹ��
	void check_n_alloc(){ if (size() == capacity()) reallocate(); }
	//��Vec�ռ�������� ÿ���ڴ�ռ䲻��ʱ������������������ �����ƶ�����Ա���µ��ڴ�ռ�
	void reallocate();
	void reallocate(size_t n);
	//˽�����ݳ�Ա
	T *elements;
	T *first_free;
	T *cap;
};
//������ⲿ�����Ա���� ��Ҫ����ģ������
template<typename T>   //�˺�����һ����ģ���еĳ�Ա����
pair<T*, T*>           //��������
Vec<T>::alloc_n_copy(const T *b, const T *e){ //������������򣺣��ǵ���Ҫʵ��<T>
	auto newdata = alloc.allocate(e - b); //����ͬ����С��һ���ڴ�ռ�
	//uninitialized_copy Ӧ���ǽ�b-e��ָ������ݸ��Ƶ���newdataΪ��ʼ��ַ�Ŀռ��� ������βָ��
	return make_pair(newdata, uninitialized_copy(b, e, newdata)); //�˴���make_pair ����pair
}

template<typename T>
void Vec<T>::free(){  
	if (elements){
	#pragma region ʹ��˵�� 
		/*while (first_free != elements){        //������Ӧ��Ҳ���� ��Ϊ���ϵ������ǲ��ı�
		alloc.destroy(--first_free);		 //first_free ��Ϊfree vector֮�� �ڴ�ռ����ͷ�
		}*/										 //��Щָ�붼��ɿ���ָ��
	#pragma endregion
		for (auto p = first_free; p != elements;)
			alloc.destroy(--p);
		alloc.deallocate(elements, cap - elements); //�ͷ�����vector�ռ�
	}
	#pragma region for_each��lambda���ʽ
	//����for_each()��Lambda��﷽ʽ�Ϻ� ����ֻ��alloc�Ǿ�̬�����������ʹ��
	//void Vec<T>::free(){
	//	for_each(elements, first_free, [](T &s){ alloc.destroy(&s); }); 
	//}
	#pragma endregion
}

template<typename T>
void Vec<T>::reallocate(){
	auto newcapacity = size() ? size() * 2 : 1; //��Ŀǰ������Ϊ0 ����Ϊ���� �������Ϊ1���ڴ�ռ�
	auto newdata = alloc.allocate(newcapacity);
	auto elem = elements; //ָ����������һ��Ԫ��
	auto dest = newdata; //ָ�����������һ��Ԫ��
	for (size_t i = 0; i != size(); ++i){ //�ƶ�size()��Ԫ�أ��Σ�
		//!�ؼ���� ��destָ��Ŀռ乹����� �������ƶ�������һ��T����
		alloc.construct(dest++, std::move(*elem++));
	}
	//�������˴�����һ��free() һ�������ƶ���Ԫ�ؾ��ͷž��ڴ�ռ� 
	free();
	//�������ݳ�Ա
	elements = newdata;
	first_free = dest;
	cap = elements + newcapacity; //�˾��ô�� �Ե��ϸ�
}
template<typename T>
void Vec<T>::reallocate(size_t n){
	auto newdata = alloc.allocate(n);
	auto elem = elements;
	auto dest = newdata;
	for (size_t i = 0; i != size(); i++)
		alloc.construct(dest++, std::move(*elem++)); //�������˸�++
	free();
	elements = newdata;
	first_free = dest;
	cap = newdata + newcapacity;
}
//�����ǿ������Ʋ�������
template<typename T>
Vec<T>::Vec(initializer_list<T> il){ //�ɱ�����б��캯��
	auto newdata = alloc_n_copy(il.begin(), il.end());
	elements = newdata.first;
	first_free = cap = newdata.second;
}
template<typename T>
Vec<T>::Vec(const Vec<T> &rhs){ //�������캯��
	auto newdata = alloc_n_copy(rhs.begin(), rhs.end()); //����һ���µĿռ䲢��lhs���󿽱�����
	elements = newdata.first;
	first_free = cap = newdata.second;
}
template<typename T>	
Vec<T>::Vec(Vec<T> &&s) _NOEXCEPT : //�ƶ����캯��
	elements(s.elements), first_free(s.first_free), cap(s.cap)
{	
	s.elements = s.first_free = s.cap = nullptr;
}
template<typename T>
Vec<T>& Vec<T>::operator=(const Vec<T> &rhs){ //������ֵ�����
	auto newdata = alloc_n_copy(rhs.begin(), rhs.end());
	free(); //���پɵĶ����ͷžɵĿռ� �Ϳ������캯����Ψһ����...
	elements = newdata.first;
	first_free = cap = newdata.second;
	return *this; 
}
template<typename T>
Vec<T>& Vec<T>::operator=(Vec<T> &&rhs) _NOEXCEPT //�ƶ���ֵ�����
{
	if (this != &rhs){ //�����Ǹ����� ��ָ��
		free();  //����Ҫ����
		elements = rhs.elements;
		first_free = rhs.first_free;
		cap = rhs.cap;
		rhs.elements = rhs.first_free = rhs.cap = nullptr; //Դ�ƶ�����ҲҪ�ÿ�
	}
	return *this;
}
template<typename T>
Vec<T>& Vec<T>::operator=(initializer_list<T> il){  //�ɱ�����б�ֵ
	auto data = alloc_n_copy(il.begin(), il.end());
	free();  //�Ϳɱ�����б��캯����Ψһ����... ��Ȼ���з���ֵ
	elements = data.first;
	first_free = cap = data.second;
	return *this;
}
//���ܺ��� 
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
	if (first_free!=elements){ //��βԪ�ز�Ϊ��
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
//���ع�ϵ�����
template<typename T>
bool operator==(const Vec<T> &lhs, const Vec<T> &rhs){
	if (lhs.size() == rhs.size()){ //������Ȳ��п������
		auto p1 = lhs.begin(), p2 = rhs.begin();
		for (; p1 != lhs.end(); ++p1, ++p2)
			if (*p1 != *p2) return false; //Ԫ�ز��ȷ���false
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
		if (*p1 < *p2) return true; //lhs��һ���ַ�����С�� ����true
		else if (*p1 > *p2) return false; //��֮ ����false
	}
	//����˴� ˵��ǰ��Ƚϵ��ַ�������� ����������һ���������ַ���ĩβ
	if (p1 == lhs.end() && p2 != rhs.end())  //p1�ȵ���ĩβ(��p2û��)˵��lhs�ĳ���С��rhs ��ȷ
		return true;
	else
		return false; //���� ����>=�� <�Ƚ�Ҳ��false��
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