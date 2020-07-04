#ifndef MY_UP_H
#define MY_UP_H
#include<iostream>
using namespace std;

template<typename T>
class UP{
public:
	UP() :p(nullptr){}
	explicit UP(T* up) :p(up){} //������û��const 
	UP(const &UP<T>) = delete; //�������캯������Ϊɾ��
	UP& operator=(const &UP<T>) = delete; //������ֵ���������Ϊɾ��
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
	T* q = p; // ���ﲻ��ֱ����pΪ�� Ҫ�������ָ��ָ��ԭ�еĶ���
	p = nullptr;
	return q;  //����ԭ����ָ��
}
#endif