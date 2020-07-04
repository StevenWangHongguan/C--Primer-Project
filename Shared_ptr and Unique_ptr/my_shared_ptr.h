#ifndef MY_SP_H
#define MY_SP_H
#include<iostream>
#include<utility> //forward����
using namespace std;

template<typename T>
class SP{
public:
	SP() :p(nullptr), use(nullptr){} //Ĭ�Ϲ��� 
	//ʹ�ö���ָ��ֱ�ӳ�ʼ�� ����SP s_p��new string()�� �ǵ�����Ϊexplicit
	explicit SP(T *pt) :p(pt), use(new size_t(1)){}  //������û��const����
	SP(const SP<T> &sp) :p(sp.p), use(p.use) { if (use) ++*use; }//�������캯��
	SP& operator=(const SP<T> &sp); //������ֵ�����
	~SP(); //��������
	T& operator*(){ return *p; } //��������ͨ�汾
	T& operator*()const{ return *p; } //�����汾 ǰ���const��Ҫ�� ���ϲ���ҪŶ
private:
	T *p; // ָ��Ԫ�ض���
	size_t *use; //ָ�������
};

template<typename T>
SP<T>& SP<T>::operator=(const SP<T> &rhs){
	if (rhs.use)	//������ֵ���� �ұ߶��������+1 ��߼�����-1
		++*rhs.use; //��Ϊ�����Ķ���Ҳ�����ǿյģ�Ĭ�Ϲ��죩������Ҫ�ж�һ��use
	if (use&&--*use == 0){ //��߶��������ָ�벻Ϊ�� �Ҹ�ֵ֮�����Ϊ0 ִ������
		delete p;
		delete use;
	}
	p = rhs.p;
	use = rhs.use;
	return *this;
}
template<typename T>
SP<T>::~SP(){
	if (use&&--*use == 0){ //���useָ�벻Ϊ�� �Ҽ�����-1���Ϊ0 ���ٶ����ͷſռ�
		delete p;
		delete use;
	}
}
// make_SP�������Խ������10��������Ȼ������Ǵ��ݸ�����T�Ĺ��캯��������һ��shared_ptr<T>�Ķ��󲢷��ء�
template<typename T, class...Args>// Args...ģ������� ��ʾ��ģ���л��ж��Class���Ͳ���
SP<T> make_SP(Args&&...args)//��ֵ�������� ���ֲ���ԭ��������Ϣ�� ...args��չArgs
{
	return SP<T>(new T(std::forward<Args>args...));
}
// ���� SP<string> sp = make_SP<string>("hello world")�� ("hello world")�ᴫ�ݸ�string�Ĺ��캯��
// SP<vector<int>> vcp = make_SP<vector<int>>(10,2); (10,2)�ᴫ�ݸ�int���͵�vector���캯�� 
#endif