/*
 * @copyright: Beijing tianzhixiang Information Technology Co., Ltd. All rights
 * reserved.
 * @website: http://www.tianzhixiang.com.cn/
 * @author: dou li yang
 * @date: 2023/8/3 11:34
 * @version: 1.0.1
 * @software: silly_utils
 * @description: 这个类中主要做加密解密功能
 * 后续补充 散列哈希[MD5、SHA1、CRC32]，对称[DES，3DES（TDEA、Triple DES），AES、，Blowfish，RC4、RC5，IDEA]，Base64、Rabbit、Escape
 * 1、对称加密：密钥只有一个，解密、解密都是这个密码，加解密速度快，典型的对称加密有DES、AES、RC4等
    2、非对称加密:密钥成对出现，分别为公钥和私钥，从公钥无法推知私钥，反之，从私钥也无法推知公钥，加密和解密使用不同的密钥，公钥加密需要私钥解密，反之，私钥加密需要公钥解密。非对称加密速度较慢，典型的非对称算法有：RSA,DSA,DSS.
    3、Hash算法，这是一种不可逆的算法，它常用于验证数据的完整性。
 */
#pragma once

#ifndef SILLY_UTILS_SILLY_ENCRYPT_H
#define SILLY_UTILS_SILLY_ENCRYPT_H
#include <su_marco.h>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1


#ifndef SILLY_ENCRYPT_AES_DEFAULT_KEY
#define SILLY_ENCRYPT_AES_DEFAULT_KEY "tzx@aes@2023"
#endif

#ifndef SILLY_ENCRYPT_AES_DEFAULT_IV
#define SILLY_ENCRYPT_AES_DEFAULT_IV "tzx@iv@2023"
#endif

class silly_encrypt
{
  public:
    /// <summary>
    /// 对字符串内容做MD5编码
    /// </summary>
    /// <param name="text"></param>
    /// <returns></returns>
    ///
    static std::string md5_text_encode(const std::string &text);

    /// <summary>
    /// 对文件内容做MD5编码
    /// </summary>
    /// <param name="filename"></param>
    /// <returns></returns>
    static std::string md5_file_encode(const std::string &file_path);

    /// <summary>
    /// 对字符串进行AES加密, 并且以base64编码结果返回
    /// </summary>
    /// <param name="text"></param>
    /// <param name="key"></param>
    /// <param name="iv"></param>
    /// <returns></returns>
    static std::string aes_text_encode_pretty(const std::string &text, const std::string &key = SILLY_ENCRYPT_AES_DEFAULT_KEY, const std::string iv = SILLY_ENCRYPT_AES_DEFAULT_IV);

    /// <summary>
    /// 与 AESEncodeTextPretty 互逆
    /// </summary>
    /// <param name="text"></param>
    /// <param name="key"></param>
    /// <param name="iv"></param>
    /// <returns></returns>
    static std::string aes_text_decode_pretty(const std::string &text, const std::string &key = SILLY_ENCRYPT_AES_DEFAULT_KEY, const std::string iv = SILLY_ENCRYPT_AES_DEFAULT_IV);

    /// <summary>
    /// Base64加密
    /// </summary>
    /// <param name="text"></param>
    /// <returns></returns>
    static std::string base64_text_encode(const std::string &text);

    /// <summary>
    /// Base64解密
    /// </summary>
    /// <param name="text"></param>
    /// <returns></returns>
    static std::string base64_text_decode(const std::string &text);
};

#endif  // SILLY_UTILS_SILLY_ENCRYPT_H
