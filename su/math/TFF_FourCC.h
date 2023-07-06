////////////////////////////////////////////////////////////////////////////////////
//
//  Copyright(c)2003-2017 北京天智祥信息科技有限公司(http://www.tianzhixiang.net)
//  All rights reserved.
//
//	@file       TFF_FourCC.h
//	@brief		TFF_FourCC header file
//	@author		gaoyang
//	@date		2017-03-08
//  @version 
//    - v1.0	2017-03-08	gaoyang	 
//
////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace common {

/** @class FourCC
 	@brief A four-character-code whitch is human readable.
	@note		
	@attention
*/
class  FourCC
{
public:
	TFF_INLINE_SPECIFIER FourCC();
	TFF_INLINE_SPECIFIER FourCC(unsigned int val);
	TFF_INLINE_SPECIFIER FourCC(const std::string& str);
	
	TFF_INLINE_SPECIFIER bool operator == (const FourCC& rhVal) const;
	TFF_INLINE_SPECIFIER bool operator != (const FourCC& rhVal) const;
	TFF_INLINE_SPECIFIER bool operator < (const FourCC& rhVal) const;
	TFF_INLINE_SPECIFIER bool operator <= (const FourCC& rhVal) const;
	TFF_INLINE_SPECIFIER bool operator > (const FourCC& rhVal) const;
	TFF_INLINE_SPECIFIER bool operator >= (const FourCC& rhVal) const;

	TFF_INLINE_SPECIFIER bool IsValid() const;
	TFF_INLINE_SPECIFIER void SetFromUInt(unsigned int val);
	TFF_INLINE_SPECIFIER unsigned int AsUInt() const;
	TFF_INLINE_SPECIFIER void SetFromString(const std::string& str);
	TFF_INLINE_SPECIFIER std::string AsString() const;

	TFF_INLINE_SPECIFIER static FourCC FromString(const std::string& str);
	TFF_INLINE_SPECIFIER static std::string ToString(const FourCC& fcc);

protected:
	/// 
	unsigned int									m_value;

};

//------------------------------------------------------------------------------
// Implementation:
//------------------------------------------------------------------------------
// Public:
FourCC::FourCC() :
	m_value(0)
{

}

FourCC::FourCC(unsigned int val) :
	m_value(val)
{

}

FourCC::FourCC(const std::string& str)
{
	SetFromString(str);
}


bool FourCC::operator == (const FourCC& rhVal) const
{
	return m_value == rhVal.m_value;
}

bool FourCC::operator != (const FourCC& rhVal) const
{
	return m_value != rhVal.m_value;
}

bool FourCC::operator < (const FourCC& rhVal) const
{
	return m_value < rhVal.m_value;
}

bool FourCC::operator <= (const FourCC& rhVal) const
{
	return m_value <= rhVal.m_value;
}

bool FourCC::operator > (const FourCC& rhVal) const
{
	return m_value > rhVal.m_value;
}

bool FourCC::operator >= (const FourCC& rhVal) const
{
	return m_value >= rhVal.m_value;
}

bool FourCC::IsValid() const
{
	return m_value != 0;
}

void FourCC::SetFromUInt(unsigned int val)
{
	assert(val != 0);
	m_value = val;
}

unsigned int FourCC::AsUInt() const
{
	return m_value;
}

void FourCC::SetFromString(const std::string& str)
{
	*this = FromString(str);
}

std::string FourCC::AsString() const
{
	return ToString(*this);
}

FourCC FourCC::FromString(const std::string& str)
{
	assert(!str.empty() && str.length() == 4);
	return FourCC((unsigned int)(str[0] | str[1] << 8 | str[2] << 16 | str[3] << 24));
}

std::string FourCC::ToString(const FourCC& fcc)
{
	assert(fcc.IsValid());
	std::string str("abcd");
	str[0] = (fcc.m_value & 0xff000000) >> 24;
	str[1] = (fcc.m_value & 0x00ff0000) >> 16;
	str[2] = (fcc.m_value & 0x0000ff00) >> 8;
	str[3] =  fcc.m_value & 0x000000ff;
	return str;
}

}
