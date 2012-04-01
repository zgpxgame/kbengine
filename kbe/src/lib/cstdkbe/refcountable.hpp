/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2012 kbegine Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
*/
/*
	���ü���ʵ����

	ʹ�÷���:
		class AA:public RefCountable
		{
		public:
			AA(){}
			~AA(){ printf("����"); }
		};
		
		--------------------------------------------
		AA* a = new AA();
		RefCountedPtr<AA>* s = new RefCountedPtr<AA>(a);
		RefCountedPtr<AA>* s1 = new RefCountedPtr<AA>(a);
		
		int i = (*s)->getRefCount();
		
		delete s;
		delete s1;
		
		ִ�н��:
			����
*/
#ifndef __REFCOUNTABLE_H__
#define __REFCOUNTABLE_H__
	
// common include
#include "cstdkbe.hpp"
//#define NDEBUG
#include <assert.h>
// windows include	
#if KBE_PLATFORM == PLATFORM_WIN32
#include <windows.h>
#else
// linux include
#include <errno.h>
#endif
	
namespace KBEngine{

class RefCountable 
{
public:
	int addRef(void) 
	{
		return ++m_refCount_;
	}

	int decRef(void) 
	{
		
		int currRef = --m_refCount_;
		assert(currRef >= 0 && "RefCountable:currRef maybe a error!");
		if (0 >= currRef)
			onRefOver();											// ���ý�����
			
		return currRef;
	}

	virtual void onRefOver(void)
	{
		delete this;
	}

	void setRefCount(int n)
	{
		m_refCount_ = n;
	}

	int getRefCount(void) const 
	{ 
		return m_refCount_; 
	}

protected:
	RefCountable(void) : m_refCount_(0) 
	{
	}

	virtual ~RefCountable(void) 
	{ 
		assert(0 == m_refCount_ && "RefCountable:currRef maybe a error!"); 
	}

protected:
	int m_refCount_;
};


class SafeRefCountable 
{
public:
	int addRef(void) 
	{
		return ::InterlockedIncrement(&m_refCount_);
	}

	int decRef(void) 
	{
		
		long currRef =::InterlockedDecrement(&m_refCount_);
		assert(currRef >= 0 && "RefCountable:currRef maybe a error!");
		if (0 >= currRef)
			onRefOver();											// ���ý�����
			
		return currRef;
	}

	virtual void onRefOver(void)
	{
		delete this;
	}

	void setRefCount(long n)
	{
		InterlockedExchange((long *)&m_refCount_, n);
	}

	int getRefCount(void) const 
	{ 
		return InterlockedExchange((long *)&m_refCount_, m_refCount_);
	}

protected:
	SafeRefCountable(void) : m_refCount_(0) 
	{
	}

	virtual ~SafeRefCountable(void) 
	{ 
		assert(0 == m_refCount_ && "SafeRefCountable:currRef maybe a error!"); 
	}

protected:
	long m_refCount_;
};

template<class T>
class RefCountedPtr 
{
public:
	RefCountedPtr(T* ptr):m_ptr_(ptr) 
	{
		if (m_ptr_)
			m_ptr_->addRef();
	}

	RefCountedPtr(RefCountedPtr<T>* refptr):m_ptr_(refptr->getObject()) 
	{
		if (m_ptr_)
			m_ptr_->addRef();
	}
	
	~RefCountedPtr(void) 
	{
		if (0 != m_ptr_)
			m_ptr_->decRef();
	}

	T& operator*()const 
	{ 
		return *m_ptr_; 
	}

	T* operator->()const 
	{ 
		return (&**this); 
	}

	T* getObject(void) const 
	{ 
		return m_ptr_; 
	}
private:
	T* m_ptr_;
};

}
#endif