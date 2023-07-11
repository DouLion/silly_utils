////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright(c)2003-2017 北京天智祥信息科技有限公司(http://www.tianzhixiang.net)
//  All rights reserved.
//
//	@file       TFF_Rectangle.h
//	@brief		TFF_Rectangle header file
//	@author		gaoyang
//	@date		2017-02-27
//  @version	
//    - v1.0	2017-02-27	gaoyang		 
//
////////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef TFF_INLINE_SPECIFIER
#define TFF_INLINE_SPECIFIER inline
#endif
#include <vector>
#include <assert.h>
/** @class Rectangle
	@brief This class wraps a rectangle shape in any type.
	@remark
*/
namespace common {
	template <class Type>
	class  Rectangle
	{
	public:
		/// Default constructor.
		TFF_INLINE_SPECIFIER Rectangle();
		/// Copy constructor.
		TFF_INLINE_SPECIFIER Rectangle(const Rectangle& other);
		/// Construct with given values.
		TFF_INLINE_SPECIFIER Rectangle(Type l, Type t, Type r, Type b);

		/// Equal operator.
		TFF_INLINE_SPECIFIER bool operator == (const Rectangle<Type>& other) const;
		/// Inequal operator.
		TFF_INLINE_SPECIFIER bool operator != (const Rectangle<Type>& other) const;
		/// Add equal operator.
		TFF_INLINE_SPECIFIER void operator += (const Rectangle<Type>& other);
		/// Sub equal operator.
		TFF_INLINE_SPECIFIER void operator -= (const Rectangle<Type>& other);
		/// And equal operator.
		TFF_INLINE_SPECIFIER void operator &= (const Rectangle<Type>& other);
		/// Or equal operator.
		TFF_INLINE_SPECIFIER void operator |= (const Rectangle<Type>& other);

		/// Add operator.
		TFF_INLINE_SPECIFIER Rectangle<Type> operator + (const Rectangle<Type>& other);
		/// Sub operator.
		TFF_INLINE_SPECIFIER Rectangle<Type> operator - (const Rectangle<Type>& other);
		/// And operator.
		TFF_INLINE_SPECIFIER Rectangle<Type> operator & (const Rectangle<Type>& other);
		/// Or operator.
		TFF_INLINE_SPECIFIER Rectangle<Type> operator | (const Rectangle<Type>& other);

		/// Set values.
		TFF_INLINE_SPECIFIER void Set(Type l, Type t, Type r, Type b);
		/// Empty the rectangle.
		TFF_INLINE_SPECIFIER void SetRectEmpty();
		/// Copy rectangle.
		TFF_INLINE_SPECIFIER void CopyRect(const Rectangle<Type>& srcRect);
		static TFF_INLINE_SPECIFIER void CopyRect(Rectangle<Type>& dstRect, const Rectangle<Type>& srcRect);
		/// True if exactly the same as another rectangle.
		TFF_INLINE_SPECIFIER bool EqualRect(const Rectangle<Type>& srcRect) const;

		/// Get the width.
		TFF_INLINE_SPECIFIER Type Width() const;
		/// Get the height.
		TFF_INLINE_SPECIFIER Type Height() const;
		/// Get center x.
		TFF_INLINE_SPECIFIER Type CenterX() const;
		/// Get center y.
		TFF_INLINE_SPECIFIER Type CenterY() const;
		/// If a point is inside the rectangle.
		TFF_INLINE_SPECIFIER bool Inside(Type x, Type y) const;
		/// If another rectangle intersects with us.
		TFF_INLINE_SPECIFIER bool InterSects(const Rectangle<Type>& other) const;
		/// If we contain another given rectangle.
		TFF_INLINE_SPECIFIER bool Contain(const Rectangle<Type>& other) const;
		/// Inflate rectangle.
		TFF_INLINE_SPECIFIER void InflateRect(const Rectangle<Type>& other);
		TFF_INLINE_SPECIFIER void InflateRect(const Type l, const Type t, const Type r, const Type b);
		TFF_INLINE_SPECIFIER void InflateRect(const Type x, const Type y);
		/// Deflate rectangle.
		TFF_INLINE_SPECIFIER void DeflateRect(const Rectangle<Type>& other);
		TFF_INLINE_SPECIFIER void DeflateRect(const Type l, const Type t, const Type r, const Type b);
		TFF_INLINE_SPECIFIER void DeflateRect(const Type x, const Type y);
		/// Translate the rectangle by moving its top and left.
		TFF_INLINE_SPECIFIER void OffsetRect(const Type x, const Type y);

		/// Set this rectangle to bounding union of two others.
		static Rectangle<Type> UnionRect(const Rectangle<Type>& r1, const Rectangle<Type>& r2);
		/// Set this rectangle to minimum of two others.
		static Rectangle<Type> SubtractRect(const Rectangle<Type>& r1, const Rectangle<Type>& r2);
		/// Inner method for merge rects.
		static void MergeRects(std::vector<Rectangle<Type>>& rectsInOut);

		Type			g_left;
		Type			g_top;
		Type			g_right;
		Type			g_bottom;
	};

	//------------------------------------------------------------------------------
	// Implementation:
	//------------------------------------------------------------------------------
	// Public:
	template <class Type>
	Rectangle<Type>::Rectangle()
	{
		// Empty.
	}

	template <class Type>
	Rectangle<Type>::Rectangle(const Rectangle& other)
	{
		g_left = other.g_left;
		g_top = other.g_top;
		g_right = other.g_right;
		g_bottom = other.g_bottom;
	}

	template <class Type>
	Rectangle<Type>::Rectangle(Type l, Type t, Type r, Type b) :
		g_left(l),
		g_top(t),
		g_right(r),
		g_bottom(b)
	{
		assert(g_left <= g_right);
		assert(g_top <= g_bottom);
	}

	template <class Type>
	bool Rectangle<Type>::operator == (const Rectangle<Type>& other) const
	{
		return EqualRect(other);
	}

	template <class Type>
	bool Rectangle<Type>::operator != (const Rectangle<Type>& other) const
	{
		return !EqualRect(other);
	}

	template <class Type>
	void Rectangle<Type>::operator += (const Rectangle<Type>& other)
	{
		InflateRect(other);
	}

	template <class Type>
	void Rectangle<Type>::operator -= (const Rectangle<Type>& other)
	{
		DeflateRect(other);
	}

	template <class Type>
	void Rectangle<Type>::operator &= (const Rectangle<Type>& other)
	{
		*this = Rectangle<Type>::SubtractRect(*this, other);
	}

	template <class Type>
	void Rectangle<Type>::operator |= (const Rectangle<Type>& other)
	{
		*this = Rectangle<Type>::UnionRect(*this, other);
	}

	template <class Type>
	Rectangle<Type> Rectangle<Type>::operator + (const Rectangle<Type>& other)
	{
		InflateRect(other);
		return *this;
	}

	template <class Type>
	Rectangle<Type> Rectangle<Type>::operator - (const Rectangle<Type>& other)
	{
		DeflateRect(other);
		return *this;
	}

	template <class Type>
	Rectangle<Type> Rectangle<Type>::operator & (const Rectangle<Type>& other)
	{
		*this = Rectangle<Type>::SubtractRect(*this, other);
		return *this;
	}

	template <class Type>
	Rectangle<Type> Rectangle<Type>::operator | (const Rectangle<Type>& other)
	{
		*this = Rectangle<Type>::UnionRect(*this, other);
		return *this;
	}

	template <class Type>
	void Rectangle<Type>::Set(Type l, Type t, Type r, Type b)
	{
		assert(l <= r);
		assert(t <= b);
		g_left = l;
		g_top = t;
		g_right = r;
		g_bottom = b;
	}

	template <class Type>
	void Rectangle<Type>::SetRectEmpty()
	{
		g_left = (Type)0;
		g_top = (Type)0;
		g_right = (Type)0;
		g_bottom = (Type)0;
	}

	template <class Type>
	void Rectangle<Type>::CopyRect(const Rectangle<Type>& srcRect)
	{
		g_left = srcRect.g_left;
		g_top = srcRect.g_top;
		g_right = srcRect.g_right;
		g_bottom = srcRect.g_bottom;
	}

	template <class Type>
	void Rectangle<Type>::CopyRect(Rectangle<Type>& dstRect, const Rectangle<Type>& srcRect)
	{
		dstRect.g_left = srcRect.g_left;
		dstRect.g_top = srcRect.g_top;
		dstRect.g_right = srcRect.g_right;
		dstRect.g_bottom = srcRect.g_bottom;
	}

	template <class Type>
	bool Rectangle<Type>::EqualRect(const Rectangle<Type>& srcRect) const
	{
		return (g_left == srcRect.g_left && g_right == srcRect.g_right && g_top == srcRect.g_top && g_bottom == srcRect.g_bottom);
	}

	template <class Type>
	Type Rectangle<Type>::Width() const
	{
		return g_right - g_left;
	}

	template <class Type>
	Type Rectangle<Type>::Height() const
	{
		return g_bottom - g_top;
	}

	template <class Type>
	Type Rectangle<Type>::CenterX() const
	{
		return (g_left + g_right) / (Type)2;
	}

	template <class Type>
	Type Rectangle<Type>::CenterY() const
	{
		return (g_top + g_bottom) / (Type)2;
	}

	template <class Type>
	bool Rectangle<Type>::Inside(Type x, Type y) const
	{
		return ((g_left <= x && x <= g_right) && (g_top <= y && y <= g_bottom));
	}

	template <class Type>
	bool Rectangle<Type>::InterSects(const Rectangle<Type>& other) const
	{
		return ((g_left < other.g_right&& other.g_left < g_right)
			&& (g_top < other.g_bottom&& other.g_top < g_bottom));
	}

	template <class Type>
	bool Rectangle<Type>::Contain(const Rectangle<Type>& other) const
	{
		return ((g_left <= other.g_left && other.g_right <= g_right)
			&& (g_top <= other.g_top && other.g_bottom <= g_bottom));
	}

	template <class Type>
	void Rectangle<Type>::InflateRect(const Rectangle<Type>& other)
	{
		g_left -= other.g_left;
		g_top -= other.g_top;
		g_right += other.g_right;
		g_bottom += other.g_bottom;
	}

	template <class Type>
	void Rectangle<Type>::InflateRect(const Type l, const Type t, const Type r, const Type b)
	{
		g_left -= l;
		g_top -= t;
		g_right += r;
		g_bottom += b;
	}

	template <class Type>
	void Rectangle<Type>::InflateRect(const Type x, const Type y)
	{
		g_left -= x;
		g_top -= y;
		g_right += x;
		g_bottom += y;
	}

	template <class Type>
	void Rectangle<Type>::DeflateRect(const Rectangle<Type>& other)
	{
		g_left += other.g_left;
		g_top += other.g_top;
		g_right -= other.g_right;
		g_bottom -= other.g_bottom;
	}

	template <class Type>
	void Rectangle<Type>::DeflateRect(const Type l, const Type t, const Type r, const Type b)
	{
		g_left += l;
		g_top += t;
		g_right -= r;
		g_bottom -= b;
	}

	template <class Type>
	void Rectangle<Type>::DeflateRect(const Type x, const Type y)
	{
		g_left += x;
		g_top += y;
		g_right -= x;
		g_bottom -= y;
	}

	template <class Type>
	void Rectangle<Type>::OffsetRect(const Type x, const Type y)
	{
		g_left += x;
		g_top += y;
		g_right += x;
		g_bottom += y;
	}

	template <class Type>
	Rectangle<Type> Rectangle<Type>::UnionRect(const Rectangle<Type>& r1, const Rectangle<Type>& r2)
	{
		Rectangle<Type> res;
		res.g_left = std::min(r1.g_left, r2.g_left);
		res.g_top = std::min(r1.g_top, r2.g_top);
		res.g_right = std::max(r1.g_right, r2.g_right);
		res.g_bottom = std::max(r1.g_bottom, r2.g_bottom);

		return res;
	}

	template <class Type>
	Rectangle<Type> Rectangle<Type>::SubtractRect(const Rectangle<Type>& r1, const Rectangle<Type>& r2)
	{
		Rectangle<Type> res;
		res.g_left = std::max(r1.g_left, r2.g_left);
		res.g_top = std::max(r1.g_top, r2.g_top);
		res.g_right = std::min(r1.g_right, r2.g_right);
		res.g_bottom = std::min(r1.g_bottom, r2.g_bottom);

		return res;
	}

	template <class Type>
	void Rectangle<Type>::MergeRects(std::vector<Rectangle<Type>>& rectsInOut)
	{
		/*
		if (rectsInOut.IsEmpty())
		{
			return;
		}

		//
		// Merge n passes until all rects of that pass have no intersections, then set the blend rects to that ones.
		//
		bool bStopPass = false;
		std::vector<Rectangle<Type>>& srcRectsOfPass = rectsInOut;
		std::vector<Rectangle<Type>> srcRectsOfOld;
		srcRectsOfOld.AppendArray(rectsInOut);
		std::vector<unsigned int> mergedIdxs;
		mergedIdxs.Clear();
		do
		{
			std::vector<Rectangle<Type>> mergedRectsThisPass;

			// For each rect in src pass, check if one rect is intersect with another one,
			// if so, merge them and put into merged rects of this pass.
			size_t numRcSrc = srcRectsOfPass.Size();
			for (unsigned int iSrc = 0; iSrc < numRcSrc; ++iSrc)
			{
				if (mergedIdxs.FindIndex(iSrc) != InvalidIndex)
				{	// This index of src rects is already merged.
					continue;
				}
				bool brcSrcMerged = false;
				Rectangle<Type>& rcSrc = srcRectsOfPass[iSrc];
				for (unsigned int iDst = 0; iDst < numRcSrc; ++iDst)
				{
					if (iDst == iSrc)
					{
						continue;
					}
					if (mergedIdxs.FindIndex(iDst) != InvalidIndex)
					{	// This index of src rects is already merged.
						continue;
					}
					Rectangle<Type>& rcDst = srcRectsOfPass[iDst];
					if (rcSrc.InterSects(rcDst))
					{
						// Merge.
						Rectangle<Type> rcMerge = Rectangle<Type>::Combine(rcSrc, rcDst);
						// Add to merged rects.
						mergedRectsThisPass.Append(rcMerge);

						// Record this index so other src rects donot check this one again.
						mergedIdxs.Append(iSrc);
						mergedIdxs.Append(iDst);

						// Src rect has been merged, so continue to check another src rect.
						brcSrcMerged = true;
						break;
					}
				}
				if (brcSrcMerged)
				{	// The rcSrc is merged, check another.
					continue;
				}
			}

			// If nothing intersects or the merged rects has only one rect, we found result.
			if (mergedIdxs.IsEmpty() || mergedRectsThisPass.Size() == 1)
			{
				bStopPass = true;
				if (mergedIdxs.IsEmpty())
				{	// Set our blend rects to src rects of this pass.
					rectsInOut = srcRectsOfPass;
				}
				else if (mergedRectsThisPass.Size() == 1)
				{	// We merged rects to one rect, set our blend rects to the merged rects.
					rectsInOut = mergedRectsThisPass;
				}
			}
			else
			{	// Need to do next pass, set merged rects to pass queue.
				srcRectsOfPass = mergedRectsThisPass;
				bStopPass = true;
			}
		} while (!bStopPass && !srcRectsOfPass.IsEmpty());

		// 将互不相交的rect加入到rectsInOut中...
		if (!mergedIdxs.IsEmpty() && mergedIdxs.Size() != srcRectsOfOld.Size())
		{
			for (unsigned int i = 0; i < srcRectsOfOld.Size(); ++i)
			{
				bool bRet = true;
				for (unsigned int j = 0; j < mergedIdxs.Size(); ++j)
				{
					if (i == mergedIdxs[j])
					{
						bRet = false;
						break;
					}
				}
				if (bRet)
				{
					rectsInOut.Append(srcRectsOfOld[i]);
				}
			}
		}
		*/
	}
}