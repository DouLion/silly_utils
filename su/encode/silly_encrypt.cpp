//
// Created by dly on 2023/8/3.
//

#include "silly_encrypt.h"

#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include <cryptopp/md5.h>
#include <cryptopp/aes.h>
#include <cryptopp/base64.h>
#include <cryptopp/crc.h>

// 这是个16位长的空串
#ifndef SILLY_ENCRYPT_AES_16_BYTE
#define SILLY_ENCRYPT_AES_16_BYTE "                "
#endif
#ifndef SILLY_ENCRYPT_AES_KEY_SIZE
#define SILLY_ENCRYPT_AES_KEY_SIZE 16
#endif

std::string silly_encrypt::md5_text_encode(const std::string &text)
{
    std::string digest;
    CryptoPP::Weak1::MD5 md5;
    CryptoPP::HashFilter hash_filter(md5);
    hash_filter.Attach(new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest), false));
    hash_filter.Put(reinterpret_cast<const unsigned char *>(text.c_str()), text.length());
    hash_filter.MessageEnd();
    return digest;
}

std::string silly_encrypt::md5_file_encode(const std::string &file_path)
{
    const size_t size = CryptoPP::Weak1::MD5::DIGESTSIZE * 2;
    // 用于存储最终的哈希值
    // CryptoPP::byte digest[size] = {0};
    std::string digest;
    try
    {
        // 创建一个MD5对象
        CryptoPP::Weak1::MD5 md5_hash;
        //// 创建文件源，计算哈希
        // CryptoPP::ArraySink asink(digest, size);
        // CryptoPP::HashFilter hfilter(md5_hash, new CryptoPP::HexEncoder(&asink));
        // CryptoPP::FileSource(file_path.c_str(), true, &hfilter);
        CryptoPP::FileSource fileSource(file_path.c_str(), true, new CryptoPP::HashFilter(md5_hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest))));
    }
    catch (const CryptoPP::Exception &e)
    {
        SU_ERROR_PRINT("AES Encode err: %s", e.what());
    }
    // return std::string(reinterpret_cast<const char *>(digest), size);
    return digest;
}

std::string silly_encrypt::aes_text_encode_pretty(const std::string &text, const std::string &key, const std::string iv)
{
    std::string cipher;
    std::string _key = key;
    _key.append(SILLY_ENCRYPT_AES_16_BYTE);
    _key = _key.substr(0, SILLY_ENCRYPT_AES_KEY_SIZE);
    std::string _iv = iv;
    _iv.append(SILLY_ENCRYPT_AES_16_BYTE);
    _iv = _iv.substr(0, SILLY_ENCRYPT_AES_KEY_SIZE);
    try
    {
        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption e;
        e.SetKeyWithIV((unsigned char *)_key.c_str(), _key.size(), (unsigned char *)_iv.c_str());

        CryptoPP::StringSource s(text,
                                 true,
                                 new CryptoPP::StreamTransformationFilter(e,
                                                                          new CryptoPP::StringSink(cipher))  // StreamTransformationFilter
        );                                                                                                   // StringSource
    }
    catch (const CryptoPP::Exception &e)
    {
        SU_ERROR_PRINT("AES Encode err: %s", e.what());
    }

    // Pretty print
    std::string encoded;
    CryptoPP::StringSource(cipher,
                           true,
                           new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encoded))  // HexEncoder
    );                                                                                     // StringSource
    return encoded;
}

std::string silly_encrypt::aes_text_decode_pretty(const std::string &text, const std::string &key, const std::string iv)
{
    std::string _key = key;
    _key.append(SILLY_ENCRYPT_AES_16_BYTE);
    _key = _key.substr(0, SILLY_ENCRYPT_AES_KEY_SIZE);
    std::string _iv = iv;
    _iv.append(SILLY_ENCRYPT_AES_16_BYTE);
    _iv = _iv.substr(0, SILLY_ENCRYPT_AES_KEY_SIZE);
    std::string encodeByte;
    CryptoPP::StringSource(text, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(encodeByte)));

    std::string recovered;
    try
    {
        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption d;
        d.SetKeyWithIV((unsigned char *)_key.c_str(), _key.size(), (unsigned char *)_iv.c_str());

        // The StreamTransformationFilter removes
        //  padding as required.
        CryptoPP::StringSource s(encodeByte, true, new CryptoPP::StreamTransformationFilter(d, new CryptoPP::StringSink(recovered)));
    }
    catch (const CryptoPP::Exception &e)
    {
        SU_ERROR_PRINT("AES Decode err: %s", e.what());
    }

    return recovered;
}

std::string silly_encrypt::base64_text_encode(const std::string &text)
{
    std::string ret;
    CryptoPP::StringSource ss(text, true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(ret)));
    return ret;
}

std::string silly_encrypt::base64_text_decode(const std::string &text)
{
    std::string ret;
    CryptoPP::StringSource ss(text, true, new CryptoPP::Base64Decoder(new CryptoPP::StringSink(ret)));
    return ret;
}
std::string silly_encrypt::crc32_text_encode(const std::string &text)
{
    /*// 创建 CRC-16 对象
    CryptoPP::CRC32 crc32;

    // 计算 CRC-16 哈希值
    CryptoPP::byte digest[CryptoPP::crc32::DIGESTSIZE];
    StringSource ss(input, true, new HashFilter(crc32, new ArraySink(digest, sizeof(digest))));

    // 将哈希值转换为十六进制字符串
    std::string output;
    HexEncoder encoder(new StringSink(output));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    return output;*/
    return std::string();
}
std::string silly_encrypt::crc32_file_encode(const std::string &file_path)
{
    return std::string();
}
std::string silly_encrypt::crc16_text_encode(const std::string &text)
{
    return std::string();
}
std::string silly_encrypt::crc16_file_encode(const std::string &file_path)
{
    return std::string();
}
