
#include "encode/silly_encode.h"

bool encode(silly_stbprp& stbprp, const std::string& from, const std::string& to)
{
    silly_encode::iconv_convert(from, to, stbprp.STCD, stbprp.STCD);
    silly_encode::iconv_convert(from, to, stbprp.STNM, stbprp.STNM);
    silly_encode::iconv_convert(from, to, stbprp.RVNM, stbprp.RVNM);
    silly_encode::iconv_convert(from, to, stbprp.HNNM, stbprp.HNNM);
    silly_encode::iconv_convert(from, to, stbprp.BSNM, stbprp.BSNM);
    silly_encode::iconv_convert(from, to, stbprp.STLC, stbprp.STLC);
    silly_encode::iconv_convert(from, to, stbprp.ADDVCD, stbprp.ADDVCD);
    silly_encode::iconv_convert(from, to, stbprp.DTMNM, stbprp.DTMNM);
    silly_encode::iconv_convert(from, to, stbprp.STTP, stbprp.STTP);
    silly_encode::iconv_convert(from, to, stbprp.FRGRD, stbprp.FRGRD);
    silly_encode::iconv_convert(from, to, stbprp.ESSTYM, stbprp.ESSTYM);
    silly_encode::iconv_convert(from, to, stbprp.BGFRYM, stbprp.BGFRYM);
    silly_encode::iconv_convert(from, to, stbprp.ATCUNIT, stbprp.ATCUNIT);
    silly_encode::iconv_convert(from, to, stbprp.ADMAUTH, stbprp.ADMAUTH);
    silly_encode::iconv_convert(from, to, stbprp.LOCALITY, stbprp.LOCALITY);
    silly_encode::iconv_convert(from, to, stbprp.STBK, stbprp.STBK);
    silly_encode::iconv_convert(from, to, stbprp.PHCD, stbprp.PHCD);
    silly_encode::iconv_convert(from, to, stbprp.USFL, stbprp.USFL);
    silly_encode::iconv_convert(from, to, stbprp.COMMENTS, stbprp.COMMENTS);
    silly_encode::iconv_convert(from, to, stbprp.HNNM0, stbprp.HNNM0);
    silly_encode::iconv_convert(from, to, stbprp.ADCD, stbprp.ADCD);
    silly_encode::iconv_convert(from, to, stbprp.ADDVCD1, stbprp.ADDVCD1);

    return true;
}
