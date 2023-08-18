/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/2/9 21:15
 * @version: 1.0.1
 * @software: HunanGeneralMessage
 * @description:
 */
#pragma once

#ifndef EZ_TOOLS_CONVERT_HPP
#define EZ_TOOLS_CONVERT_HPP

#include <codecvt>
#include <string>
#include <cstring>
 // iconv
#include <iconv.h>
namespace silly_conv
{

	static std::wstring Cxx11PlusS2WS(const std::string& str)
	{
#if IS_WIN32
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.from_bytes(str);
#else
		std::wstring aa;
		return aa;
#endif
	}

	static std::string Cxx11PlusWS2S(const std::wstring& wstr)
	{
#if IS_WIN32
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
#else
		return "";
#endif
	}
}



namespace silly_code
{
	enum Encode { eeANSI = 1, eeUTF16_LE, eeUTF16_BE, eeUTF8_BOM, eeUTF8 };

	/// <summary>
	/// 判断中文字符串是否为UTF8编码
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <returns></returns>
	static Encode IsUTF8(const uint8_t* data, size_t size)
	{
		bool bAnsi = true;
		uint8_t ch = 0x00;
		int32_t nBytes = 0;
		for (auto i = 0; i < size; i++)
		{
			ch = *(data + i);
			if ((ch & 0x80) != 0x00)
			{
				bAnsi = false;
			}
			if (nBytes == 0)
			{
				if (ch >= 0x80)
				{
					if (ch >= 0xFC && ch <= 0xFD)
					{
						nBytes = 6;
					}
					else if (ch >= 0xF8)
					{
						nBytes = 5;
					}
					else if (ch >= 0xF0)
					{
						nBytes = 4;
					}
					else if (ch >= 0xE0)
					{
						nBytes = 3;
					}

					else if (ch >= 0xC0)
					{
						nBytes = 2;
					}
					else
					{
						return Encode::eeANSI;
					}
					nBytes--;
				}
			}
			else
			{
				if ((ch & 0xC0) != 0x80)
				{
					return Encode::eeANSI;
				}
				nBytes--;
			}
		}
		if (nBytes > 0 || bAnsi)
		{
			return Encode::eeANSI;
		}
		return Encode::eeUTF8;

	}

	/// <summary>
	/// 检查字符串的编码格式
	/// </summary>
	/// <param name="data"></param>
	/// <param name="size"></param>
	/// <returns></returns>
	static Encode DetectEncode(const uint8_t* data, size_t size)
	{
		if (size > 2 && data[0] == 0xFF && data[1] == 0xFE)
		{
			return Encode::eeUTF16_LE;
		}
		else if (size > 2 && data[0] == 0xFE && data[1] == 0xFF)
		{
			return Encode::eeUTF16_BE;
		}
		else if (size > 3 && data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
		{
			return Encode::eeUTF8_BOM;
		}
		else
		{
			return IsUTF8(data, size);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// ICONV
//////////////////////////////////////////////////////////////////////////

/*
	It provides support for the encodings:

	European languages
	ASCII, ISO-8859-{1,2,3,4,5,7,9,10,13,14,15,16}, KOI8-R, KOI8-U, KOI8-RU, CP{1250,1251,1252,1253,1254,1257}, CP{850,866,1131}, Mac{Roman,CentralEurope,Iceland,Croatian,Romania}, Mac{Cyrillic,Ukraine,Greek,Turkish}, Macintosh
	Semitic languages
	ISO-8859-{6,8}, CP{1255,1256}, CP862, Mac{Hebrew,Arabic}
	Japanese
	EUC-JP, SHIFT_JIS, CP932, ISO-2022-JP, ISO-2022-JP-2, ISO-2022-JP-1, ISO-2022-JP-MS
	Chinese
	EUC-CN, HZ, GBK, CP936, GB18030, EUC-TW, BIG5, CP950, BIG5-HKSCS, BIG5-HKSCS:2004, BIG5-HKSCS:2001, BIG5-HKSCS:1999, ISO-2022-CN, ISO-2022-CN-EXT
	Korean
	EUC-KR, CP949, ISO-2022-KR, JOHAB
	Armenian
	ARMSCII-8
	Georgian
	Georgian-Academy, Georgian-PS
	Tajik
	KOI8-T
	Kazakh
	PT154, RK1048
	Thai
	ISO-8859-11, TIS-620, CP874, MacThai
	Laotian
	MuleLao-1, CP1133
	Vietnamese
	VISCII, TCVN, CP1258
	Platform specifics
	HP-ROMAN8, NEXTSTEP
	Full Unicode
	UTF-8
	UCS-2, UCS-2BE, UCS-2LE
	UCS-4, UCS-4BE, UCS-4LE
	UTF-16, UTF-16BE, UTF-16LE
	UTF-32, UTF-32BE, UTF-32LE
	UTF-7
	C99, JAVA
	Full Unicode, in terms of uint16_t or uint32_t (with machine dependent endianness and alignment)
	UCS-2-INTERNAL, UCS-4-INTERNAL
	Locale dependent, in terms of `char' or `wchar_t' (with machine dependent endianness and alignment, and with OS and locale dependent semantics)
	char, wchar_t
	The empty encoding name "" is equivalent to "char": it denotes the locale dependent character encoding.
	When configured with the option --enable-extra-encodings, it also provides support for a few extra encodings:
	European languages
	CP{437,737,775,852,853,855,857,858,860,861,863,865,869,1125}
	Semitic languages
	CP864
	Japanese
	EUC-JISX0213, Shift_JISX0213, ISO-2022-JP-3
	Chinese
	BIG5-2003 (experimental)
	Turkmen
	TDS565
	Platform specifics
	ATARIST, RISCOS-LATIN1
	It can convert from any of these encodings to any other, through Unicode conversion.
*/


namespace silly_conv
{
	/// <summary>
	///
	/// </summary>
	/// <param name="fromcode"></param>
	/// <param name="tocode"></param>
	/// <param name="text"></param>
	/// <returns></returns>
	static std::string encode_convert(const char* fromcode, const char* tocode, const char* text)
	{
		char* tmp = (char*)text;
		size_t lenSrc = strlen(tmp);
		size_t lenDst = lenSrc * 5;
		char* out = (char*)malloc(lenDst);
		memset(out, 0, lenDst);
		char* pFreeOut = out;
		iconv_t cd = iconv_open(tocode, fromcode);
		size_t ret = iconv(cd, &tmp, &lenSrc, &out, &lenDst);
		if (ret == -1)
		{
			return "";
		}
		std::string retStr(pFreeOut);
		iconv_close(cd);
		free(pFreeOut);
		return retStr;
	}

	// GBK转utf-8, SQLITE存储中文时需要, 因为项目的编码方式是UNICODE
	static std::string GBK2UTF8(const char* text)
	{
		return encode_convert("GBK", "UTF-8", text);
	}

	static std::string UTF82GBK(const char* text)
	{
		return encode_convert("UTF-8", "GBK", text);
	}

	static std::string Unicode2GBK(const char* text)
	{
		return encode_convert("EUC-CN", "GBK", text);
	}

	static std::string Unicode2UTF8(const char* text)
	{
		return encode_convert("EUC-CN", "UTF-8", text);
	}

	static std::string GBK2UTF8(const std::string& text)
	{
		return encode_convert("GBK", "UTF-8", text.c_str());
	}

	static std::string UTF82GBK(const std::string& text)
	{
		return encode_convert("UTF-8", "GBK", text.c_str());
	}

	static std::string Unicode2GBK(const std::string& text)
	{
		return encode_convert("EUC-CN", "GBK", text.c_str());
	}

	static std::string Unicode2UTF8(const std::string& text)
	{
		return encode_convert("EUC-CN", "UTF-8", text.c_str());
	}


	static bool IsTextUTF8(const char* str, int length)
	{
		int i;
		int nBytes = 0;             // UFT8可用1-6个字节编码,ASCII用一个字节
		unsigned char chr;
		bool bAllAscii = true;      // 如果全部都是ASCII, 说明不是UTF-8            
		for (i = 0; i < length; i++)
		{
			chr = *(str + i);
			if ((chr & 0x80) != 0 && bAllAscii)  // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx
				bAllAscii = false;
			if (nBytes == 0)         //如果不是ASCII码,应该是多字节符,计算字节数
			{
				if (chr >= 0x80)
				{
					if (chr > 0xFD)
						return false;
					else if (chr >= 0xFC && chr <= 0xFD) // 6字节 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
						nBytes = 6;
					else if (chr >= 0xF8) // 5字节 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
						nBytes = 5;
					else if (chr >= 0xF0) // 4字节 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
						nBytes = 4;
					else if (chr >= 0xE0) // 3字节 1110xxxx 10xxxxxx 10xxxxxx 
						nBytes = 3;
					else //if(chr >= 0xC0)  2字节 110xxxxx 10xxxxxx gbk编码
						return false;

					nBytes--;
				}
			}
			else //多字节符的非首字节,应为 10xxxxxx
			{
				if ((chr & 0xC0) != 0x80)
				{
					return false;
				}
				nBytes--;
			}
		}

		if (nBytes > 0 || bAllAscii) //违返规则 或者 全部都是ASCII, 说明不是UTF-8
		{
			return false;
		}

		return true;
	}

	static void Byte2Hstr(const unsigned char* Bstr, const long Len, char* Hstr)
	{
		for (int i = 0; i < Len; i++)
		{
			sprintf(&Hstr[2 * i], "%02X", Bstr[i]);
		}
		Hstr[Len * 2] = 0;
	}

	static void Hstr2Byte(const char* str, unsigned char* Hstr)
	{
		int Len = (int)(strlen(str) / 2);
		for (int i = 0; i < Len; ++i)
		{
			if (isdigit(str[2 * i]))
			{
				Hstr[i] = (str[2 * i] - '0') * 16;
			}
			else
			{
				Hstr[i] = (toupper(str[2 * i]) - 'A' + 10) * 16;
			}

			if (isdigit(str[2 * i + 1]))
			{
				Hstr[i] += str[2 * i + 1] - '0';
			}
			else
			{
				Hstr[i] += toupper(str[2 * i + 1]) - 'A' + 10;
			}
		}

		Hstr[Len] = 0;
	}

}
#endif //EZ_TOOLS_CONVERT_HPP
