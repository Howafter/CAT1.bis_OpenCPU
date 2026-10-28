#include "l_rtc_port.h"
#include "time.h"

int l_rtc_get_time(struct tm *tm)
{
    liot_rtc_time_s rtc_tm = {0};

    if (tm == NULL) {
        return -1;
    }
    if (liot_rtc_get_time(&rtc_tm) != LIOT_RTC_SUCCESS) {
        return -1;
    }

    /* liot_rtc_time_s uses a full year and a 1-based month; struct tm does not. */
    tm->tm_year = rtc_tm.tm_year - 1900;
    tm->tm_mon = rtc_tm.tm_mon - 1;
    tm->tm_mday = rtc_tm.tm_mday;
    tm->tm_hour = rtc_tm.tm_hour;
    tm->tm_min = rtc_tm.tm_min;
    tm->tm_sec = rtc_tm.tm_sec;
    tm->tm_wday = rtc_tm.tm_wday;

    return 0;
}
