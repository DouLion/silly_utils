#pragma once

#include <boost/noncopyable.hpp>
#include <boost/assert.hpp>

class singleton_module :
	public boost::noncopyable
{
private:
	static bool & get_lock(){
		static bool lock = false;
		return lock;
	}
public:
	//    static const void * get_module_handle(){
	//        return static_cast<const void *>(get_module_handle);
	//    }
	static void lock(){
		get_lock() = true;
	}
	static void unlock(){
		get_lock() = false;
	}
	static bool is_locked() {
		return get_lock();
	}
};

template<class T>
class singleton_wrapper : public T
{
public:
	static bool m_is_destroyed;
	~singleton_wrapper(){
		m_is_destroyed = true;
	}
};

template<class T>
bool singleton_wrapper< T >::m_is_destroyed = false;


template <class T>
class Singleton : public singleton_module
{
private:
	static T & instance;
	// include this to provoke instantiation at pre-execution time
	static void Use(T const &) {}
	static T & GetInstance() {
		static singleton_wrapper< T > t;
		// refer to instance, causing it to be instantiated (and
		// initialized at startup on working compilers)
		BOOST_ASSERT(! singleton_wrapper< T >::m_is_destroyed);
		Use(instance);
		return static_cast<T &>(t);
	}
public:
	static T * GetMutableInstance(){
		BOOST_ASSERT(! is_locked());
		return &GetInstance();
	}
	static const T & GetConstInstance(){
		return GetInstance();
	}
	static bool IsDestroyed(){
		return singleton_wrapper< T >::m_is_destroyed;
	}
};

template<class T>
T & Singleton< T >::instance = Singleton< T >::GetInstance();
