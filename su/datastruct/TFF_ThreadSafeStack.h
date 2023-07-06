//////////////////////////////////////////////////////////////////////////
//
//	@file		TFF_ThreadSafeStack.h
//	@brief	TFF_ThreadSafeStack header file
//	@author		gaoyang
//  @version    1.0
//	@date		2017-03-01
//
//////////////////////////////////////////////////////////////////////////

#pragma once


#include <stack>
#include <mutex>

/** @class ThreadSafeStack
	@brief The class of thread safe stack.
	@remark
*/  
template<typename V>
class  ThreadSafeStack
{
	typedef std::stack<V>   StackType;
public:
	/// Default Constructor.
	ThreadSafeStack(void){};
	/// Destructor.
	virtual ~ThreadSafeStack(void){};

	/// Push stack.
	bool Push(const V& value)
	{
		std::scoped_lock  lock(m_recMutex);
		m_stack.push(value);

		return true;
	}

	/// Pop stack.
	bool Pop(V& value)
	{
		std::scoped_lock  lock(m_recMutex);
		if (!m_stack.empty())
		{
			value = m_stack.top();
			m_stack.pop();

			return true;
		}

		return true;
	}

	/// Get stack size.
	int GetSize()
	{
		int size = m_stack.size();
		return size;
	}

	/// Whether is empty.
	bool IsEmpty()
	{
		bool bEmpty = m_stack.empty();
		return bEmpty;
	}

	/// Clear stack.
	bool Clear()
	{
		std::scoped_lock  lock(m_recMutex);
		while (!m_stack.empty())
		{
			V& value = m_stack.top();
			m_stack.pop();
		}
		return true;
	}

protected:
	/// Boost recursive mutex.
	std::mutex										m_recMutex;
	/// Stack structure.
	StackType										m_stack;

};


