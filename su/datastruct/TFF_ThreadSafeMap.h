//////////////////////////////////////////////////////////////////////////
//
//	@file		TFF_ThreadSafeMap.h
//	@brief	TFF_ThreadSafeMap header file
//	@author		gaoyang
//  @version    1.0
//	@date		2017-03-01
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <map>
#include <vector>
#include <mutex>

/** @class ThreadSafeMap
        @brief The class of thread safe map.
        @remark
*/
template <typename K, typename V>
class ThreadSafeMap
{
    typedef std::map<K, V> MapType;

  public:
    /// Default constructor.
    ThreadSafeMap(void){};
    /// Destructor.
    virtual ~ThreadSafeMap(void){};

    /// Add item.
    void AddItem(const K& key, const V& value)
    {
        std::scoped_lock lock(m_recMutex);
        m_map[key] = value;
    }

    /// Get item.
    bool GetItem(const K& key, V& value)
    {
        bool bFind = false;
        std::scoped_lock lock(m_recMutex);
        typename MapType::iterator iter = m_map.find(key);
        if (iter != m_map.end())
        {
            value = iter->second;
            bFind = true;
        }

        return bFind;
    }

    /// Whether the key is exist.
    bool IsExistItem(const K& key)
    {
        if (m_map.find(key) != m_map.end())
        {
            return true;
        }

        return false;
    }

    /// Remove key.
    void RemoveKey(const K& key)
    {
        std::scoped_lock lock(m_recMutex);
        m_map.erase(key);
    }

    /// Clear map.
    void Clear()
    {
        std::scoped_lock lock(m_recMutex);
        m_map.clear();
    }

    /// Get map size.
    int Size()
    {
        std::scoped_lock lock(m_recMutex);
        return m_map.size();
    }

    /// Get keys.
    std::vector<K> GetKeys()
    {
        std::vector<K> keys;
        typename std::map<K, V>::iterator it;
        for (it = m_map.begin(); it != m_map.end(); ++it)
        {
            keys.push_back(it->first);
        }
        return keys;
    }

  protected:
    /// Boost recursive mutex.
    std::mutex m_recMutex;
    /// Map structure.
    MapType m_map;
};
