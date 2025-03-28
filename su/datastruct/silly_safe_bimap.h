
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

template <typename KT, typename VT>
class silly_safe_bimap
{
    /// 默认以key为主
  public:
    using key_iterator = typename std::unordered_map<KT, VT>::iterator;
    using const_key_iterator = typename std::unordered_map<KT, VT>::const_iterator;
    using value_iterator = typename std::unordered_map<VT, KT>::iterator;
    using const_value_iterator = typename std::unordered_map<VT, KT>::const_iterator;

  public:
    silly_safe_bimap() = default;
    ~silly_safe_bimap() = default;

    /// <summary>
    /// 插入一对数据, 如果key或者value已经存在, 则返回false
    /// </summary>
    /// <param name="key"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    bool insert(const KT& key, const VT& value)
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

    /// <summary>
    /// 是否包含key
    /// </summary>
    /// <param name="key"></param>
    /// <returns></returns>
    bool contains(const KT& key) const
    {
        return m_key_map.find(key) != m_key_map.end();
    }

    /// <summary>
    /// 是否包含Value
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    bool contains_value(const VT& value) const
    {
        return m_value_map.find(value) != m_value_map.end();
    }

    /// <summary>
    /// 根据key删除数据
    /// </summary>
    /// <param name="key"></param>
    /// <returns></returns>
    bool erase(const KT& key)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_key_map.find(key);
        if (it == m_key_map.end())
        {
            return false;
        }
        VT value = it->second;
        m_key_map.erase(it);
        m_value_map.erase(value);
        return true;
    }

    /// <summary>
    /// 根据value删除数据
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    bool erase_value(const VT& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_value_map.find(value);
        if (it == m_value_map.end())
        {
            return false;
        }
        KT key = it->second;
        m_key_map.erase(key);
        m_value_map.erase(it);
        return true;
    }

    /// <summary>
    /// 根据key获取value
    /// </summary>
    /// <param name="key"></param>
    /// <returns></returns>
    VT get(const KT& key) const
    {
        auto it = m_key_map.find(key);
        if (it == m_key_map.end())
        {
            return VT();
        }
        return it->second;
    }

    /// <summary>
    /// 根据key 获取value, 带验证
    /// </summary>
    /// <param name="value"></param>
    /// <param name="key"></param>
    /// <returns></returns>
    bool get(const VT& value, KT& key) const
    {
        auto it = m_value_map.find(value);
        if (it == m_value_map.end())
        {
            return false;
        }
        key = it->second;
        return true;
    }

    /// <summary>
    /// 根据value获取key
    /// </summary>
    /// <param name="key"></param>
    /// <param name="value"></param>
    /// <returns></returns>
    bool get_value(const KT& key, VT& value) const
    {
        auto it = m_key_map.find(key);
        if (it == m_key_map.end())
        {
            return false;
        }
        value = it->second;
        return true;
    }

    /// <summary>
    /// 根据value获取key
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    KT get(const VT& value) const
    {
        auto it = m_value_map.find(value);
        if (it == m_value_map.end())
        {
            return KT();
        }
        return it->second;
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_key_map.clear();
        m_value_map.clear();
    }

    void for_each(std::function<void(const KT&, const VT&)> func) const
    {
        for (auto& item : m_key_map)
        {
            func(item.first, item.second);
        }
    }

    void for_each(std::function<void(VT&, KT&)> func)
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

    const_key_iterator find(const KT& key) const
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

    const_key_iterator find(const KT& key)
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

    const_value_iterator find_value(const VT& value) const
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

    const_value_iterator find_value(const VT& value)
    {
        return m_value_map.find(value);
    }

  protected:
    std::unordered_map<KT, VT> m_key_map;
    std::unordered_map<VT, KT> m_value_map;
    std::mutex m_mutex;
};

#endif  // SILLY_UTILS_SILLY_SAFE_BIMAP_H