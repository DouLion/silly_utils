/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2025-12-24
 * @file: RZ2W_L.cpp
 * @description: 水位库容关系线, 这个似乎更加适合链表结构, std::list
 *              水位单位 米
 *              库容单位 立方米
 * @version: v1.0.1 2025-12-24 dou li yang
 */
#ifndef RZ2W_L_H
#define RZ2W_L_H
#include <hydro/line/ALGO_L.h>
class RZ2W_L
{
public:
    RZ2W_L() = default;
    RZ2W_L(const double& unit);
    ~RZ2W_L() = default;

    /**
     * @brief: 添加一组水位库容关系,
     *  这里应该更具Z的值动态插入,而不是直接放到最后
     * @param z 水位
     * @param w 库容
     */
    void AddData(const double& z, const double& w);

    /**
     * @brief: 设置数据
     * @param: ZWs 水位库容关系线 [z0,w0, z1, w1 .... zn, wn]
     */
    void SetData(const std::vector<double>& ZWs);

    /**
     * @brief: 设置数据
     * @param: Zs 水位
     * @param: Ws 库容 与水位一一对应
     */
    void SetData(const std::vector<double>& Zs, const std::vector<double>& Ws);

    /**
     * @brief: 清空数据
     */
    void Clear();

    /**
     * @brief: 根据z的由小到大排序
     */
    void Sort();

    /**
     * 根据水位插值计算库容
     * @param z 水位
     * @param type 插值方式
     * @return
     */
    double GetW( const double& z, const int& type = 4) const;

    /**
     * 根据库容, 插值计算水位
     * @param w 库容
     * @param type 插值方式
     * @return
     */
    double GetZ( const double& w, const int& type = 4) const ;
protected:
    std::vector<double> m_Zs; // 水位
    std::vector<double> m_Ws; // 库容
    double m_unit = 1e6; // 单位 这个现在似乎没有吊用
};
#endif  // RZ2W_L_H
