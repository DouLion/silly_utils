////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright(c)2003-2018 北京天智祥信息科技有限公司(http://www.tianzhixiang.net)
//  All rights reserved.
//
//	@file       TFF_Screen.h
//	@brief		TFF_Screen header file
//	@author		gaoyang
//	@date		2017-02-14
//  @version
//    - v1.0	2017-02-14	gaoyang
//
////////////////////////////////////////////////////////////////////////////////////

#pragma once

/** @class ScreenPoint
        @brief The class of screen point.
        @remark
*/
class ScreenPoint
{
  public:
    /// Default constructor.
    TFF_INLINE_SPECIFIER ScreenPoint();
    TFF_INLINE_SPECIFIER ScreenPoint(const unsigned char layer, const long long left, const long long top);
    /// Destructor.
    TFF_INLINE_SPECIFIER virtual ~ScreenPoint();

  public:
    /// Left position.
    long long left;
    /// Top position.
    long long top;
    /// Layer number.
    unsigned char layer;
};

/** @class ScreenRect
        @brief The class of screen rect.
        @remark
*/
class ScreenRect
{
  public:
    /// Default constructor.
    TFF_INLINE_SPECIFIER ScreenRect();
    TFF_INLINE_SPECIFIER ScreenRect(const unsigned char layer, const long long left, const long long top, const long long right, const long long bottom);
    /// Destructor.
    TFF_INLINE_SPECIFIER virtual ~ScreenRect();

  public:
    /// Left position.
    long long left;
    /// Top position.
    long long top;
    /// Right position.
    long long right;
    /// Bottom position.
    long long bottom;
    /// Layer number.
    unsigned char layer;
};

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:
ScreenPoint::ScreenPoint() : layer(0), left(0), top(0)
{
}

ScreenPoint::ScreenPoint(const unsigned char layer, const long long left, const long long top) : layer(layer), left(left), top(top)
{
}

ScreenPoint::~ScreenPoint()
{
}

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:
ScreenRect::ScreenRect()
{
    left = top = right = bottom = layer = 0;
}

ScreenRect::ScreenRect(const unsigned char layer, const long long left, const long long top, const long long right, const long long bottom) : layer(layer), left(left), top(top), right(right), bottom(bottom)
{
}

ScreenRect::~ScreenRect()
{
}
