/*
 * @copyright: Beijing TianZhiXiang Information Technology Co., Ltd. All rights
 * reserved. 北京天智祥信息科技有限公司版权所有
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2024-12-13
 * @file: silly_egm.h
 * @description: Gravity Field Model of the Earth 地球重力场文件数据
 * @version: v1.0.1 2024-12-13 dou li yang
 */
#ifndef SILLY_UTILS_SILLY_EGM_H
#define SILLY_UTILS_SILLY_EGM_H
#include <su_marco.h>
#include <files/silly_file.h>
#include <files/silly_memory_map.h>

class silly_egm
{
  public:
    class header
    {
      public:
        std::vector<std::string> infos;
        // 参照# Scale 0.003
        double scale = 0.003;
        // 参照 # Offset -108
        double offset = -108.0;
        double min_lat = -90.0;
        double max_lat = 90.;
        double min_lon = -180.;
        double max_lon = 180.;
        // 参照 # Origin 90N 0E
        double origin_lat = 90.;
        double origin_lon = 0.;
        size_t rows = 0;
        size_t cols = 0;
        size_t dlen = 1;  // 格点数据长度
    };

  public:
    silly_egm() = default;
    ~silly_egm();
    bool open(const std::string& file);

    /// <summary>
    /// 获取大地水准面高差, 见文件下方说明
    /// 使用双线性差值
    /// </summary>
    /// <param name="lgtd">经度, -180~180, 0~180为东经, -180~-0为西经</param>
    /// <param name="lttd">纬度, -90~90, -90为南纬, 90为北纬</param>
    /// <returns></returns>
    double geoid(const double& lgtd, const double& lttd);

    /// <summary>
    /// 获取正高, 根据ellipsoid 计算  ellipsoid - geoid(lgtd, lttd)
    /// </summary>
    /// <param name="lgtd"></param>
    /// <param name="lttd"></param>
    /// <param name="ellipsoid"></param>
    /// <returns></returns>
    double orthometric(const double& lgtd, const double& lttd, const double& ellipsoid = 458.284);

    bool close();

  private:
    // TODO: 头文件没有完全解析
    bool read_header();

  private:
    header m_headers;
    size_t m_doffs = 0;  // 数据块起始位置在文件中的偏移
    silly::file::memory_map m_mmap;
    double m_lat_ratio = 0;
    double m_lon_ratio = 0;
};

/* 常用高程类型说明

一、高程height
        定义：地面点到高度起算面的垂直距离。“高程”是测绘用词，通俗的理解，高程其实就是海拔高度。
            在测量学中，高程的定义是某地表点在地球引力方向上 的高度，也就是重心所在地球引力线的高度。因此，地球表面上 每个点高程的方向都是不同的。
                世界各国采用的高程系统主要有两类： 正高系统和正常高系统，其所对应的高程名称分别为海拔高和近似海拔高，统称为高程。
    1.1 水准原点 leveling origin
        定义：高程起算的基准点。
            过去我国采用青岛验潮站1950-1956年观测成果求得的黄海平均海水面作为高程的零点，称为“1956年黄海高程系”。后经复查，发现该高程系验潮资料过短，准确性较差，改用青岛验潮站1950-1979年的观测资料重新推算，并命名为“1985年国家高程基准”。国家水准点设于青岛市观象山，作为我国高程测量的依据。它的高程是以1985年国家高程基准”所定的平均海水面为零点测算而得，废止了原来“1956年黄海高程系”的高程。
    1.2 高程异常height anomaly
        定义：似大地水准面至地球椭球面的高度。
    1.3 正高orthometric height
        定义：地面点沿该点的重力线到 大地水准面的距离。
            是天文地理坐标(Ψ，λ，Hg)的高程分量。
    1.4 正常高normal height
    释文：正常高是指从一地面点沿过此点的正常重力线到似大地水准面的距离。我国规定采用的高程系统是正常高系统。如果不是进行科学研究，只是一般使用，正常高系 统结果在国内也可以称为海拔高度。正常高系统和正高系统是有区别的，主要是由于重力场的影响不同，重力线就会产生一些偏移。
    正常重力线：正常重力场中的力线。
    正常重力场：由于地球内部质量分布的不规则性，致使地球重力场不是一个按简单规律变化的力场。但从总的趋势看，地球非常接近于一个旋转椭球，因此可将实际地球规则化，称为正常地球，同它相应的地球重力场称为正常重力场。它的重力位称为正常位U，重力称为正常重力γ0。在正常重力场中也有一簇正常位水准面，它们都是扁球面。某点的正常重力方向是正常重力场重力线的切线方向。
    1.5 大地高 geodetic height;ellipsoidal height
        定义：一点沿椭球法线到椭球面的距离。
            释文：大地高是指从一地面点沿过此点的地球椭球面的
                法线到地球椭球面的距离。是大地地理坐标（B， L，H）的高程分量H。

                    二、基准面
    2.1 大地水准面 geoid
        由静止海水面并向大陆延伸所形成的不规则的封闭曲面。它是重力等位面，即物体沿该面运动时，重力不做功（如水在这个面上是不会流动的）。大地水准面是指与全球平均海平面（或静止海水面）相重合的水准面。大地水准面是描述地球形状的一个重要物理参考面，也是海拔高程系统的起算面。大地水准面的确定是通过确定它与参考椭球面的间距-大地水准面差距（对于似大地水准面而言，则称为高程异常）来实现的。
    2.2 似大地水准面quasi-geoid
        从地面点沿正常重力线量取正常高所得端点构成的封闭曲面。似大地水准面严格说不是水准面，但接近于水准面，只是用于计算的辅助面。它与大地水准面不完全吻合，差值为正常高与正高之差。但在海洋面上时，似大地水准面与大地水准面重合。
            建立一个高精度、三维、动态、多功能的国家空间坐标基准框架、国家高程基准框架、国家重力基准框架，以及由GPS、水准、重力等综合技术精化的高精度、高分辨率似大地水准面。该框架工程的建成，将为基础测绘、数字中国地理空间基础框架、区域沉降监测、环境预报与防灾减灾、国防建设、海洋科学、气象预报、地学研究、交通、水利、电力等多学科研究与应用提供必要的测绘服务，具有重大的科学意义。
                精化大地水准面对于测绘工作有重要意义：首先，大地水准面或似大地水准面是获取地理空间信息的高程基准面。其次，GPS（全球定位系统）技术结合高精度高分辨率大地水准面模型，可以取代传统的水淮测量方法测定正高或正常高，真正实现GPS技术对几何和物理意义上的三维定位功能。再次，在现今GPS定位时代，精化区域性大地水准面和建立新一代传统的国家或区域性高程控制网同等重要，也是一个国家或地区建立现代高程基准的主要任务，以此满足国家经济建设和测绘科学技术的发展以及相关地学研究的需要。近年来，我国经济发达地区及中、小城市，在地形图测绘方面，对厘米级似大地水准面的需求十分迫切。高精度的似大地水准面结合GPS定位技术所获得的三维坐标中的大地高分离求解正常高，可以改变传统高程测量作业模式，满足1：1万、1：5000甚至更大比例尺测图的迫切需要，加快数字中国、数字区域、数字城市等的建设，不但节约大量人力物力，产生巨大的经济效益，而且具有特别重要的科学意义和社会效益。
                    大地高等于正常高与高程异常之和，GPS测定的是大地高，要求解正常高必须先知道高程异常。在局部GPS网中巳知一些点 的高程异常（它由GPS水准算得）， 考虑地球重力场模型，利用多面函数拟合法求定其它点的高程异常和正常高。
    2.3 参考椭球面 surface of reference ellipsoid
        处理大地测量成果而采用的与地球大小、形状接近并进行定位椭球体表面。参考椭球面是测量、计算的基准面。地球体从整体上看，十分接近于一个规则的旋转椭球体。地球椭球由三个椭球元素：长半轴，短半轴和扁率表示。形状、大小一定且已经与大地体作了最佳拟合的地球椭球称为参考椭球。我国的最佳拟合点，也称为大地原点，位于陕西省西安市泾阳县永乐镇。各国为处理大地测量的成果，往往根据本国及其他国家的天文、大地、重力测量结果采用适合本国的椭球参数并将其定位。
            我国在成立之前采用海福特椭球参数，新中国成立之初采用克拉索夫斯基椭球参数（其大地原点在前苏联，对我国密合不好，越往南方误差越大）。目前采用的是1975年国际大地测量学与物理学联合会（IUGG）推荐的椭球，在我国称为“1980年国家大地坐标系”。坐标原点即是前面提到的“陕西省咸阳市泾阳县永乐镇”。2008年7月1日我国启动了2000国家大地坐标系，计划用8~10年完成现行国家大地坐标系到2000国家大地坐标系的过渡与转换工作。

*/

#endif  // SILLY_UTILS_SILLY_EGM_H
