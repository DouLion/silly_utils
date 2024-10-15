
#include "encode/silly_encode.h"

bool encode(silly_stbprp& stbprp, const std::string& src_encode, const std::string& dst_encode)
{
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.STCD, stbprp.STCD);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.STNM, stbprp.STNM);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.RVNM, stbprp.RVNM);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.HNNM, stbprp.HNNM);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.BSNM, stbprp.BSNM);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.STLC, stbprp.STLC);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.ADDVCD, stbprp.ADDVCD);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.DTMNM, stbprp.DTMNM);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.STTP, stbprp.STTP);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.FRGRD, stbprp.FRGRD);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.ESSTYM, stbprp.ESSTYM);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.BGFRYM, stbprp.BGFRYM);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.ATCUNIT, stbprp.ATCUNIT);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.ADMAUTH, stbprp.ADMAUTH);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.LOCALITY, stbprp.LOCALITY);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.STBK, stbprp.STBK);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.PHCD, stbprp.PHCD);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.USFL, stbprp.USFL);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.COMMENTS, stbprp.COMMENTS);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.HNNM0, stbprp.HNNM0);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.ADCD, stbprp.ADCD);
    silly_encode::iconv_convert(src_encode, dst_encode, stbprp.ADDVCD1, stbprp.ADDVCD1);

    return true;
}
