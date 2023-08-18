/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/7/11 16:03
 * @version: 1.0.1
 * @software: silly_utils
 * @description:
 */
// 获取变量名称的宏
#define SU_VAR_NAME(var) #var

#ifndef PI
#define PI (3.1415926535897932384626433832795028841971693993751f)
#endif
#define SU_PI PI
#define SU_2PI (SU_PI * 2.)

// 自然指数e
#ifndef BASE_E
#define BASE_E 2.71828182845904523536
#endif
#define SU_BASE_E BASE_E

#ifndef TINY
#define TINY (0.0000001f)
#endif
#define SU_TINY TINY

// #define		EPS 与opencv的冲突
#define		SU_EPS .1e-12

/** earth radius */
#ifndef EARTH_RADIUS
#define EARTH_RADIUS 6378.137
#endif
#define SU_EARTH_RADIUS EARTH_RADIUS

#define LN_2   (0.693147180559945f)
#define SQRT_2   (1.4142135623730950488016887242097f)

#define M_PI	3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_2PI (M_PI*2.)


#define SU_RGB(r, g, b)				((unsigned int)(((unsigned char)(r)|((unsigned int)((unsigned char)(g))<<8))|(((unsigned int)(unsigned char)(b))<<16)))
#define SU_ARGB(a, r, g, b)			(unsigned int)( ((a)&0xff) << 24 | ((r)&0xff) << 16 | ((g)&0xff) << 8 | (b&0xff) )
#define SU_RGBA(r, g, b, a)			SU_ARGB( a, r, g, b )
#define SU_XRGB(r, g, b)			SU_ARGB( 0xff, r, g, b )

#ifndef __FILENAME__
#define __FILENAME__ (strrchr("/" __FILE__, '/') + 1)
#endif

#ifndef NDEBUG
#define SU_DEBUG_PRINT printf
#else
#define SU_DEBUG_PRINT //
#endif

#define SU_RETURN_OK printf("OK.\n"); return 0;
#define SU_RETURN_ERR printf("ERR.\n"); return 1;

#ifndef SU_MAX
#define SU_MAX(a, b)	(((a) > (b)) ? (a) : (b))
#endif

#ifndef SU_MIN
#define SU_MIN(a, b)	(((a) < (b)) ? (a) : (b))
#endif

//转化小端序的int值
#define CONVERT_LITTLE_ENDIAN_LONG(src_ptr) (src_ptr[3] << 24 | (src_ptr[2] << 16) | src_ptr[1] << 8 | src_ptr[0])
//转化小端序的int值
#define CONVERT_LITTLE_ENDIAN_INT(src_ptr) (src_ptr[3] << 24 | (src_ptr[2] << 16) | src_ptr[1] << 8 | src_ptr[0])
//转化小端序的short值
#define CONVERT_LITTLE_ENDIAN_SHORT(src_ptr) (src_ptr[1] << 8) + src_ptr[0]
