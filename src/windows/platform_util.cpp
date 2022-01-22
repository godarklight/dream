
#include "platform_util.h"

#include <windows.h>

time_t timegm(struct tm *tm)
{
    SYSTEMTIME st;
    st.wYear = tm->tm_year+1900;
    st.wMonth = tm->tm_mon+1;
    st.wDay = tm->tm_mday;
    st.wHour = 0;
    st.wMinute = 0;
    st.wSecond = 0;
    st.wMilliseconds = 0;
    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);
    ULARGE_INTEGER uli;
    uli.LowPart = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    return (time_t)(uli.QuadPart/10000000 - 11644473600);
}

