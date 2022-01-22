
#include "../util/Pacer.h"

/* The FILETIME structure is a 64-bit value representing
 * the number of 100-nanosecond intervals since January 1, 1601.
 */

CPacer::CPacer(uint64_t ns)
{
    FILETIME ft;
    hTimer = CreateWaitableTimer(nullptr, true, TEXT("CPacerTimer"));
    if (hTimer==nullptr)
    {
        throw "Create Timer failed ";
    }
    GetSystemTimeAsFileTime(&ft);
    interval = ns/100;
    LARGE_INTEGER dueTime;
    dueTime.LowPart = ft.dwLowDateTime;
    dueTime.HighPart = ft.dwHighDateTime;
    dueTime.QuadPart += interval;;
    if (!SetWaitableTimer(hTimer, &dueTime, 0, nullptr, nullptr, 0))
    {
        throw "Set Timer failed ";
    }
    timekeeper = (uint64_t)dueTime.QuadPart;
}

CPacer::~CPacer()
{
    CancelWaitableTimer(hTimer);
}

uint64_t CPacer::nstogo()
{
    FILETIME ft;
    LARGE_INTEGER liNow;
    GetSystemTimeAsFileTime(&ft);
    liNow.LowPart = ft.dwLowDateTime;
    liNow.HighPart = ft.dwHighDateTime;
    return (uint64_t)100*(timekeeper - liNow.QuadPart);
}

void CPacer::wait()
{
    if (WaitForSingleObject(hTimer, INFINITE) !=WAIT_OBJECT_0)
    {
        throw "Wait on Timer failed ";
    }
    /* and re-arm */
    timekeeper += interval;
    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = timekeeper;
    if (!SetWaitableTimer(hTimer, &liDueTime, 0, nullptr, nullptr, 0))
    {
        throw "Set Timer failed ";
    }
}
