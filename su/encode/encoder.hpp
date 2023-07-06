/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2022/12/12 9:45
 * @version: 1.0.1
 * @software: HunanGeneralMessage
 * @description:
 */
#pragma once

#ifndef EZ_TOOLS_ENCODER_H
#define EZ_TOOLS_ENCODER_H

// CryptoPP
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include <cryptopp/md5.h>
#include <cryptopp/aes.h>
#include <cryptopp/base64.h>

//////////////////////////////////////////////////////////////////////////
// marco
//////////////////////////////////////////////////////////////////////////

// aes 加密密码编码长度为16
#define TZX_ENCODE_AES_16_BYTE  "                "
#define TZX_ENCODE_AES_KEY_SIZE 16

//////////////////////////////////////////////////////////////////////////
// MD5
//////////////////////////////////////////////////////////////////////////

namespace TZXMD5
{
	/// <summary>
	/// 对字符串内容做MD5编码
	/// </summary>
	/// <param name="text"></param>
	/// <returns></returns>
	/// 
	static std::string Md5EncodeText(const unsigned char* text, const int& len)
	{

		std::string digest;
		CryptoPP::Weak1::MD5 md5;
		CryptoPP::HashFilter hashfilter(md5);
		hashfilter.Attach(new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest), false));
		hashfilter.Put(reinterpret_cast<const unsigned char*>(text), len);
		hashfilter.MessageEnd();
		return digest;
	}

	static std::string Md5EncodeText(const std::string& text)
	{

		std::string digest;
		CryptoPP::Weak1::MD5 md5;
		CryptoPP::HashFilter hashfilter(md5);
		hashfilter.Attach(new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest), false));
		hashfilter.Put(reinterpret_cast<const unsigned char*>(text.c_str()), text.length());
		hashfilter.MessageEnd();
		return digest;
	}

	/// <summary>
	/// 对文件内容做MD5编码
	/// </summary>
	/// <param name="filename"></param>
	/// <returns></returns>
	static std::string Md5EncodeFile(const std::string& filename)
	{
		std::string digest;
		CryptoPP::Weak1::MD5 md5;
		CryptoPP::HashFilter hashfilter(md5);
		hashfilter.Attach(new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest), false));
		CryptoPP::FileSource(filename.c_str(), true, &hashfilter);
		return digest;
	}
}


//////////////////////////////////////////////////////////////////////////
// AES
//////////////////////////////////////////////////////////////////////////

namespace TZXAES
{
	/// <summary>
	/// 对字符串进行AES加密, 并且以base64编码结果返回
	/// </summary>
	/// <param name="text"></param>
	/// <param name="key"></param>
	/// <param name="iv"></param>
	/// <returns></returns>
	static std::string AESEncodeTextPretty(const std::string& text, const std::string& key = "tzx@aes@2023", const std::string iv = "tzx@iv@2023")
	{
		std::string cipher;
		std::string _key = key + TZX_ENCODE_AES_16_BYTE;
		_key = _key.substr(0, 16);
		std::string _iv = iv + TZX_ENCODE_AES_16_BYTE;
		_iv = _iv.substr(0, 16);
		try
		{
			CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
			e.SetKeyWithIV((unsigned char*)_key.c_str(), _key.size(), (unsigned char*)_iv.c_str());

			// The StreamTransformationFilter removes
			//  padding as required.
			CryptoPP::StringSource s(text, true,
					new CryptoPP::StreamTransformationFilter(e,
							new CryptoPP::StringSink(cipher)
					) // StreamTransformationFilter
			); // StringSource
		}
		catch (const CryptoPP::Exception& e)
		{
			std::cout << "AES Encode err:" << e.what();
		}

		// Pretty print
		std::string encoded;
		CryptoPP::StringSource(cipher, true,
				new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encoded)
				) // HexEncoder
		); // StringSource
		return encoded;
	}

	/// <summary>
	/// 与 AESEncodeTextPretty 互逆
	/// </summary>
	/// <param name="text"></param>
	/// <param name="key"></param>
	/// <param name="iv"></param>
	/// <returns></returns>
	static std::string AESDecodeTextPretty(const std::string& text, const std::string& key = "tzx@aes@2023", const std::string iv = "tzx@iv@2023")
	{
		std::string _key = key + TZX_ENCODE_AES_16_BYTE;
		_key = _key.substr(0, 16);
		std::string _iv = iv + TZX_ENCODE_AES_16_BYTE;
		_iv = _iv.substr(0, 16);
		std::string encodeByte;
		CryptoPP::StringSource(text, true, new CryptoPP::Base64Decoder(
				new CryptoPP::StringSink(encodeByte)
		));

		std::string recovered;
		try
		{
			CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;
			d.SetKeyWithIV((unsigned char*)_key.c_str(), _key.size(), (unsigned char*)_iv.c_str());

			// The StreamTransformationFilter removes
			//  padding as required.
			CryptoPP::StringSource s(encodeByte, true,
					new CryptoPP::StreamTransformationFilter(d,
							new CryptoPP::StringSink(recovered)
					) // StreamTransformationFilter
			); // StringSource
		}
		catch (const CryptoPP::Exception& e)
		{
			std::cout << "AES Decode err: " << e.what() << std::endl;
		}

		return recovered;
	}

}

//////////////////////////////////////////////////////////////////////////
// Base64
//////////////////////////////////////////////////////////////////////////

namespace TZXBASE64
{
	/// <summary>
	/// Base64加密
	/// </summary>
	/// <param name="text"></param>
	/// <returns></returns>
	static std::string Base64Encode(const std::string& text)
	{
		std::string estr;
		CryptoPP::StringSource ss(text, true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(estr)));
		return estr;
	}

	/// <summary>
	/// Base64解密
	/// </summary>
	/// <param name="text"></param>
	/// <returns></returns>
	static std::string Base64Decode(const std::string& text)
	{
		std::string dstr;
		CryptoPP::StringSource ss(text, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(dstr)));
		return dstr;
	}
}

namespace TZXURL
{
	/// <summary>
	/// URL编码
	/// </summary>
	/// <param name="src"></param>
	/// <returns></returns>
	static std::string UrlEncode(const std::string& src)
	{
		static    char hex[] = "0123456789ABCDEF";
		std::string dst;

		for (size_t i = 0; i < src.size(); i++)
		{
			unsigned char ch = src[i];
			if (isalnum(ch))
			{
				dst += ch;
			}
			else
				if (src[i] == ' ')
				{
					dst += '+';
				}
				else
				{
					unsigned char c = static_cast<unsigned char>(src[i]);
					dst += '%';
					dst += hex[c / 16];
					dst += hex[c % 16];
				}
		}
		return dst;
	}

	/// <summary>
	/// URL解码
	/// </summary>
	/// <param name="src"></param>
	/// <returns></returns>
	static std::string UrlDecode(const std::string& src)
	{
		std::string dst;
		size_t srclen = src.size();
		for (size_t i = 0; i < srclen; i++)
		{
			if (src[i] == '%')
			{
				if (isxdigit(src[i + 1]) && isxdigit(src[i + 2]))
				{
					char c1 = src[++i];
					char c2 = src[++i];
					c1 = c1 - 48 - ((c1 >= 'A') ? 7 : 0) - ((c1 >= 'a') ? 32 : 0);
					c2 = c2 - 48 - ((c2 >= 'A') ? 7 : 0) - ((c2 >= 'a') ? 32 : 0);
					dst += (unsigned char)(c1 * 16 + c2);
				}
			}
			else
			{
				if (src[i] == '+')
				{
					dst += ' ';
				}
				else
				{
					dst += src[i];
				}
			}
		}

		return dst;
	}
}

#endif //EZ_TOOLS_ENCODER_H
