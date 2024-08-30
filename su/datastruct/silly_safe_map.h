/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-30
 * @file: silly_safe_map.h
 * @description: silly_safe_map 类声明
 * @version: v1.0.1 2024-08-30 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SAFE_MAP_H
#define SILLY_UTILS_SILLY_SAFE_MAP_H
#include <su_marco.h>
template <typename K, typename V>
class silly_safe_map
{
    typedef std::map<K, V> MapType;

  public:
    /// Default constructor.
    silly_safe_map(void){};
    /// Destructor.
    virtual ~silly_safe_map(void){};

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

#endif  // SILLY_UTILS_SILLY_SAFE_MAP_H
