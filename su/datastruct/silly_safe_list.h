/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-08-30
 * @file: silly_safe_list.h
 * @description: silly_safe_list 类声明
 * @version: v1.0.1 2024-08-30 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_SAFE_LIST_H
#define SILLY_UTILS_SILLY_SAFE_LIST_H
#include <su_marco.h>
template <typename V>
class silly_safe_list
{
    typedef std::list<V> ListType;

  public:
    /// Default constructor.
    silly_safe_list(void){};
    /// Destructor.
    virtual ~silly_safe_list(void){};

    /// Add item.
    bool AddItem(const V& value)
    {
        std::scoped_lock lock(m_recMutex);
        m_list.push_back(value);

        return true;
    }

    /// Remove item.
    bool RemoveItem(const V& value)
    {
        std::scoped_lock lock(m_recMutex);
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
    std::mutex m_recMutex;
    /// List structure.
    ListType m_list;
};

#endif  // SILLY_UTILS_SILLY_SAFE_LIST_H
