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
    typedef std::map<K, V> type;

  public:
    /// Default constructor.
    silly_safe_map(void){};
    /// Destructor.
    virtual ~silly_safe_map(void){};

    /// Add item.
    void add(const K& key, const V& value)
    {
        std::scoped_lock lock(m_mutex);
        m_map[key] = value;
    }

    /// Get item.
    bool get(const K& key, V& value)
    {
        bool bFind = false;
        std::scoped_lock lock(m_mutex);
        typename type::iterator iter = m_map.find(key);
        if (iter != m_map.end())
        {
            value = iter->second;
            bFind = true;
        }

        return bFind;
    }

    /// Whether the key is exist.
    bool exist(const K& key)
    {
        if (m_map.find(key) != m_map.end())
        {
            return true;
        }

        return false;
    }

    /// Remove key.
    void erase(const K& key)
    {
        std::scoped_lock lock(m_mutex);
        m_map.erase(key);
    }

    /// Clear map.
    void clear()
    {
        std::scoped_lock lock(m_mutex);
        m_map.clear();
    }

    /// Get map size.
    size_t size()
    {
        return m_map.size();
    }

    /// <summary>
    /// 获取所有的主键
    /// </summary>
    /// <returns></returns>
    std::vector<K> keys()
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
    std::mutex m_mutex;
    /// Map structure.
    type m_map;
};

#endif  // SILLY_UTILS_SILLY_SAFE_MAP_H
