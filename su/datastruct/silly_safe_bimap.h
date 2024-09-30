
/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-11
 * @file: silly_safe_bimap.h
 * @description: silly_safe_bimap  双向映射的简单实现
 * @version: v1.0.2 2024-09-11 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SAFE_BIMAP_H
#define SILLY_UTILS_SILLY_SAFE_BIMAP_H
#include <su_marco.h>

template <typename KeyType, typename ValueType>
class silly_safe_bimap
{
    /// 默认以key为主
  public:
    using key_iterator = typename std::unordered_map<KeyType, ValueType>::iterator;
    using const_key_iterator = typename std::unordered_map<KeyType, ValueType>::const_iterator;
    using value_iterator = typename std::unordered_map<ValueType, KeyType>::iterator;
    using const_value_iterator = typename std::unordered_map<ValueType, KeyType>::const_iterator;

  public:
    silly_safe_bimap() = default;
    ~silly_safe_bimap() = default;

    bool insert(const KeyType& key, const ValueType& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_key_map.find(key) != m_key_map.end())
        {
            return false;
        }
        if (m_value_map.find(value) != m_value_map.end())
        {
            return false;
        }
        m_key_map[key] = value;
        m_value_map[value] = key;
        return true;
    }

    bool contains(const KeyType& key) const
    {
        return m_key_map.find(key) != m_key_map.end();
    }

    bool contains_value(const ValueType& value) const
    {
        return m_value_map.find(value) != m_value_map.end();
    }

    bool erase(const KeyType& key)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_key_map.find(key);
        if (it == m_key_map.end())
        {
            return false;
        }
        ValueType value = it->second;
        m_key_map.erase(it);
        m_value_map.erase(value);
        return true;
    }

    bool erase_value(const ValueType& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_value_map.find(value);
        if (it == m_value_map.end())
        {
            return false;
        }
        KeyType key = it->second;
        m_key_map.erase(key);
        m_value_map.erase(it);
        return true;
    }

    bool get(const ValueType& value, KeyType& key) const
    {
        auto it = m_value_map.find(value);
        if (it == m_value_map.end())
        {
            return false;
        }
        key = it->second;
        return true;
    }

    bool get_value(const KeyType& key, ValueType& value) const
    {
        auto it = m_key_map.find(key);
        if (it == m_key_map.end())
        {
            return false;
        }
        value = it->second;
        return true;
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_key_map.clear();
        m_value_map.clear();
    }

    void for_each(std::function<void(const KeyType&, const ValueType&)> func) const
    {
        for (auto& item : m_key_map)
        {
            func(item.first, item.second);
        }
    }

    void for_each(std::function<void(ValueType&, KeyType&)> func)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& item : m_value_map)
        {
            func(item.first, item.second);
        }
    }

    size_t size() const
    {
        return m_key_map.size();
    }

    bool check() const
    {
        return m_key_map.size() == m_value_map.size();
    }

    const_key_iterator end() const
    {
        return m_key_map.end();
    }

    const_key_iterator begin() const
    {
        return m_key_map.begin();
    }

    const_key_iterator find(const KeyType& key) const
    {
        return m_key_map.find(key);
    }

    const_key_iterator end()
    {
        return m_key_map.end();
    }

    const_key_iterator begin()
    {
        return m_key_map.begin();
    }

    const_key_iterator find(const KeyType& key)
    {
        return m_key_map.find(key);
    }

    const_value_iterator end_value() const
    {
        return m_value_map.end();
    }

    const_value_iterator begin_value() const
    {
        return m_value_map.begin();
    }

    const_value_iterator find_value(const ValueType& value) const
    {
        return m_value_map.find(value);
    }

    const_value_iterator end_value()
    {
        return m_value_map.end();
    }

    const_value_iterator begin_value()
    {
        return m_value_map.begin();
    }

    const_value_iterator find_value(const ValueType& value)
    {
        return m_value_map.find(value);
    }

  protected:
    std::unordered_map<KeyType, ValueType> m_key_map;
    std::unordered_map<ValueType, KeyType> m_value_map;
    std::mutex m_mutex;
};

#endif  // SILLY_UTILS_SILLY_SAFE_BIMAP_H