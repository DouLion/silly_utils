////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright(c)2003-2018 北京天智祥信息科技有限公司(http://www.tianzhixiang.net)
//  All rights reserved.
//
//	@file       TFF_Pyramid.h
//	@brief		TFF_Pyramid header file
//	@author		gaoyang
//	@date		2017-02-14
//  @version
//    - v1.0	2017-02-14	gaoyang
//
////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

/** @class PyramidPoint
        @brief The class of pyramid point.
        @remark
*/
class PyramidPoint
{
  public:
    /// Default constructor.
    TFF_INLINE_SPECIFIER PyramidPoint();

    TFF_INLINE_SPECIFIER PyramidPoint(const unsigned char layer, const long long lrow, const long long lcol, const unsigned char* pBuffer = NULL, const unsigned int lsize = 0);
    /// Destructor.
    TFF_INLINE_SPECIFIER virtual ~PyramidPoint();

    /// Assign operator.
    TFF_INLINE_SPECIFIER PyramidPoint& operator=(const PyramidPoint& pp);
    /// Equal operator.
    TFF_INLINE_SPECIFIER bool operator==(const PyramidPoint& pp) const;
    /// Inequal operator.
    TFF_INLINE_SPECIFIER bool operator!=(const PyramidPoint& pp) const;
    /// Less than operator.
    TFF_INLINE_SPECIFIER bool operator<(const PyramidPoint& pp) const;

    /// Whether is valid.
    TFF_INLINE_SPECIFIER bool IsValid() const;
    /// Convert object to string.
    TFF_INLINE_SPECIFIER std::string ToString() const;
    /// Convert string to object.
    TFF_INLINE_SPECIFIER void FromString(const std::string& ppStr);

  public:
    ///	Layer number.
    signed char layer;
    ///	Row number.
    long long lrow;
    ///	Column number.
    long long lcol;
    /// Data buffer.
    unsigned char* pBuffer;
    /// Data length.
    unsigned int lsize;
    /// Offset X.
    unsigned char offsetX;
    /// Offset Y.
    unsigned char offsetY;
};

/** @class PyramidRect
        @brief The class of pyramid rect.
        @remark
*/
class PyramidRect
{
  public:
    /// Default constructor.
    TFF_INLINE_SPECIFIER PyramidRect();

    TFF_INLINE_SPECIFIER PyramidRect(const unsigned char layer, const long long begRow, const long long begCol, const long long endRow, const long long endCol);
    /// Destructor.
    TFF_INLINE_SPECIFIER virtual ~PyramidRect();

    /// Whether pp is in current rect.
    TFF_INLINE_SPECIFIER bool IsInRect(const PyramidPoint& pp);
    /// Whether is valid.
    TFF_INLINE_SPECIFIER bool IsValid() const;
    /// Convert to pyramid point array.
    TFF_INLINE_SPECIFIER std::vector<PyramidPoint> ToPyramidPoints() const;
    /// Convert to pyramid point string array.
    TFF_INLINE_SPECIFIER std::vector<std::string> ToPyramidPointString() const;

  public:
    /// Layer number.
    signed char layer;
    /// Begin row number.
    long long begRow;
    /// Begin column number.
    long long begCol;
    /// End row number.
    long long endRow;
    /// End column number.
    long long endCol;
};

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:
PyramidPoint::PyramidPoint()
{
    layer = -1;
    lrow = lcol = -1;
    lsize = 0;
    pBuffer = NULL;
    offsetX = 0;
    offsetY = 0;
}

PyramidPoint::PyramidPoint(const unsigned char layer, const long long lrow, const long long lcol, const unsigned char* pBuffer, const unsigned int lsize)
    : layer(layer), lrow(lrow), lcol(lcol), pBuffer((unsigned char*)pBuffer), lsize(lsize), offsetX(0), offsetY(0)
{
}

PyramidPoint::~PyramidPoint() = default;

PyramidPoint& PyramidPoint::operator=(const PyramidPoint& pp)
{
    this->layer = pp.layer;
    this->lcol = pp.lcol;
    this->lrow = pp.lrow;
    this->pBuffer = pp.pBuffer;
    this->lsize = pp.lsize;
    this->offsetX = pp.offsetX;
    this->offsetY = pp.offsetY;
    return *this;
}

bool PyramidPoint::operator==(const PyramidPoint& pp) const
{
    return (this->layer == pp.layer) && (this->lcol == pp.lcol) && (this->lrow == pp.lrow);
}

bool PyramidPoint::operator!=(const PyramidPoint& pp) const
{
    return (this->layer != pp.layer) || (this->lcol != pp.lcol) || (this->lrow != pp.lrow);
}

bool PyramidPoint::operator<(const PyramidPoint& pp) const
{
    if (this->layer < pp.layer)
    {
        return true;
    }
    else if (this->layer > pp.layer)
    {
        return false;
    }
    else
    {
        if (this->lrow < pp.lrow)
        {
            return true;
        }
        else if (this->lrow > pp.lrow)
        {
            return false;
        }
        else
        {
            if (this->lcol < pp.lcol)
            {
                return true;
            }
            else if (this->lcol >= pp.lcol)
            {
                return false;
            }
        }
    }

    return true;

    // return (this->layer <= pp.layer) && (this->lcol <= pp.lcol) && (this->lrow <= pp.lrow);
}

bool PyramidPoint::IsValid() const
{
    return !(this->layer == -1 && this->lrow == -1 && this->lcol == -1 && this->pBuffer == NULL && this->lsize == 0);
}

std::string PyramidPoint::ToString() const
{
    char buf[256] = {0};
    sprintf(buf, "%d_%lld_%lld", (int)this->layer, this->lrow, this->lcol);
    return buf;
}

void PyramidPoint::FromString(const std::string& ppStr)
{
    std::vector<std::string> tmpVec;
    boost::split(tmpVec, ppStr, boost::is_any_of("_"));
    size_t num = tmpVec.size();
    if (num >= 3)
    {
        this->layer = boost::lexical_cast<unsigned char>(tmpVec[0]);
        this->lrow = boost::lexical_cast<long long>(tmpVec[1]);
        this->lcol = boost::lexical_cast<long long>(tmpVec[2]);
    }
}

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:
PyramidRect::PyramidRect()
{
    begRow = begCol = endRow = endCol = layer = 0;
}

PyramidRect::PyramidRect(const unsigned char layer, const long long begRow, const long long begCol, const long long endRow, const long long endCol) : layer(layer), begRow(begRow), begCol(begCol), endRow(endRow), endCol(endCol)
{
}

PyramidRect::~PyramidRect()
{
}

bool PyramidRect::IsInRect(const PyramidPoint& pp)
{
    if (pp.lcol >= begCol && pp.lcol <= endCol && pp.lrow >= begRow && pp.lrow <= endRow)
    {
        return true;
    }

    return false;
}

bool PyramidRect::IsValid() const
{
    if (layer >= 0 && begRow <= endRow && begCol <= endCol)
    {
        return true;
    }

    return false;
}

std::vector<PyramidPoint> PyramidRect::ToPyramidPoints() const
{
    std::vector<PyramidPoint> ppArr;
    PyramidPoint pp;
    for (long long j = begRow; j <= endRow; ++j)
    {
        for (long long k = begCol; k <= endCol; ++k)
        {
            pp.layer = layer;
            pp.lrow = j;
            pp.lcol = k;
            ppArr.push_back(pp);
        }
    }

    return ppArr;
}

std::vector<std::string> PyramidRect::ToPyramidPointString() const
{
    std::vector<std::string> ppStrArr;
    PyramidPoint pp;
    for (long long j = begRow; j <= endRow; ++j)
    {
        for (long long k = begCol; k <= endCol; ++k)
        {
            pp.layer = layer;
            pp.lrow = j;
            pp.lcol = k;
            ppStrArr.push_back(pp.ToString());
        }
    }

    return ppStrArr;
}
