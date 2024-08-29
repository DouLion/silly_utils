//
// Created by dly on 2023/7/10.
//

#include "silly_gzip.h"
#include <zlib.h>

int silly_gzip::compress(const std::string& s_src, std::string& s_dst)
{
    return 0;
}

int silly_gzip::decompress(const std::string& s_src, std::string& s_dst)
{
    return 0;
}

int silly_gzip::compress(const char* c_in_val, const unsigned int& i_in_len, char** c_out_val, unsigned int* i_out_len)
{
    z_stream c_stream;
    int err = 0;
    int windowBits = 15;
    int GZIP_ENCODING = 16;

    if (c_in_val && i_in_len > 0)
    {
        c_stream.zalloc = (alloc_func)0;
        c_stream.zfree = (free_func)0;
        c_stream.opaque = (voidpf)0;
        if (deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits | GZIP_ENCODING, 8, Z_DEFAULT_STRATEGY) != Z_OK)
            return -1;
        c_stream.next_in = (Bytef*)*c_in_val;
        c_stream.avail_in = i_in_len;
        c_stream.next_out = (Bytef*)*c_out_val;
        c_stream.avail_out = *i_out_len;
        while (c_stream.avail_in != 0 && c_stream.total_out < *i_out_len)
        {
            if (deflate(&c_stream, Z_NO_FLUSH) != Z_OK)
                return -1;
        }
        if (c_stream.avail_in != 0)
            return c_stream.avail_in;
        for (;;)
        {
            if ((err = deflate(&c_stream, Z_FINISH)) == Z_STREAM_END)
                break;
            if (err != Z_OK)
                return -1;
        }
        if (deflateEnd(&c_stream) != Z_OK)
            return -1;
        return c_stream.total_out;
    }
    return -1;
    return 0;
}

int silly_gzip::decompress(const char* c_in_val, const unsigned int& i_in_len, char** c_out_val, unsigned int* i_out_len)
{
    // 测试
    z_stream strm;
    strm.zalloc = NULL;
    strm.zfree = NULL;
    strm.opaque = NULL;

    strm.avail_in = i_in_len;
    strm.avail_out = *i_out_len;
    strm.next_in = (Bytef*)c_in_val;
    strm.next_out = (Bytef*)*c_out_val;

    int err = -1, ret = -1;
    err = inflateInit2(&strm, MAX_WBITS + 16);
    if (err == Z_OK)
    {
        err = inflate(&strm, Z_FINISH);
        if (err == Z_STREAM_END)
        {
            ret = strm.total_out;
        }
        else
        {
            inflateEnd(&strm);
            return err;
        }
    }
    else
    {
        inflateEnd(&strm);
        return err;
    }
    inflateEnd(&strm);
    return err;
}
