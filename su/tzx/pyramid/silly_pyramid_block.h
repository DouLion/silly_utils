/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-09-24
 * @file: silly_pyramid_block.h
 * @description: silly_pyramid_block 类声明
 * @version: v1.0.1 2024-09-24 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_PYRAMID_BLOCK_H
#define SILLY_UTILS_SILLY_PYRAMID_BLOCK_H

namespace silly
{
namespace pyramid
{
class block
{
  public:
    uint32_t zoom{0};
    uint64_t row{0};
    uint64_t col{0};
    // 位置信息
    uint64_t pos{0};
    uint64_t offset{0};
    // 数据信息
    uint32_t size{0};
    std::string data;
  public:
    void create()
    {
        data.resize(size);
    }
};

}  // namespace pyramid
}  // namespace silly

#endif  // SILLY_UTILS_SILLY_PYRAMID_BLOCK_H
