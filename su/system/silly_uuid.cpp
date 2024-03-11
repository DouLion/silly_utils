#include "silly_uuid.h"

#include <stdint.h>
#include <array>
#include <string>

#include <sstream>
#include <iomanip>
#include <ios>
#ifdef IS_WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif

std::string silly_uuid::random_uuid()
{
    char buff[64] = {0};
#ifdef IS_WIN32

    GUID guid;
    HRESULT hr = CoCreateGuid(&guid);
    sprintf(buff, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

#else
    uuid_t uu;
    uuid_generate_random(uu);
    uuid_unparse_upper(uu, buff);
#endif
    return std::string(buff);
}
