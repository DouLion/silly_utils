//////////////////////////////////////////////////////////////////////////
//
//	@file		TFF_ThreadSafeList.h
//	@brief	TFF_ThreadSafeList header file
//	@author		gaoyang
//  @version    1.0
//	@date		2017-03-01
//
//////////////////////////////////////////////////////////////////////////

#pragma once


#include <list>
#include <mutex>

/** @class ThreadSafeList
	@brief The class of thread safe list.
	@remark
*/  
template<typename V>
class  ThreadSafeList
{
	typedef std::list<V>   ListType;
public:
	/// Default constructor.
	ThreadSafeList(void){};
	/// Destructor.
	virtual ~ThreadSafeList(void){};

	/// Add item.
	bool AddItem(const V& value)
	{
		std::scoped_lock  lock(m_recMutex);
		m_list.push_back(value);

		return true;
	}

	/// Remove item.
	bool RemoveItem(const V& value)
	{
		std::scoped_lock  lock(m_recMutex);
		m_list.remove(value);

		return true;
	}

	/// Get list size.
	int GetSize()
	{
		int size = m_list.size();
		return size;
	}

	/// Whether is empty.
	bool IsEmpty()
	{
		bool bEmpty = m_list.empty();
		return bEmpty;
	}

	/// Clear list.
	bool Clear()
	{
		std::scoped_lock lock(m_recMutex);
		m_list.clear();

		return true;
	}

protected:
	/// Boost recursive mutex.
	std::mutex									m_recMutex;
	/// List structure.
	ListType									m_list;

};



