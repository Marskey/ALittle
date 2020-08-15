
#ifndef _ALITTLE_TIMEHELPER_H_
#define _ALITTLE_TIMEHELPER_H_

#include <string>
#include <time.h>

namespace ALittle
{

const int ONE_MINUTE_SECONDS = 60;						// ÿ���Ӷ�Ӧ������
const int ONE_HOUR_SECONDS = 60 * ONE_MINUTE_SECONDS;	// ÿСʱ��Ӧ������
const int ONE_DAY_SECONDS = 24 * ONE_HOUR_SECONDS;		// һ�������
const int ONE_WEEK_DAY = 7;								// һ����Ҫ������
const int ONE_WEEK_SECONDS = 7 * ONE_DAY_SECONDS;		// һ����Ҫ������

class TimeHelper
{
public:
	// ��ȡ��ǰʱ�䣬��λ����
	static long long GetCurMSTime();
	// ��ȡ��ǰʱ�䣬��λ��
	static time_t GetCurTime();
	// ��ȡ����0���ʱ�䣬��λ��
	static time_t GetTodayBeginTime();
    static time_t CalcTodayBeginTime(time_t cur_time);
    // ��ȡ�ڶ���0���ʱ�䣬��λ��
	static time_t GetNextTodayBeginTime();

    // ��ȡ�����0��ʱ�䣬��λ��
    static time_t GetMonthBeginTime();
    static time_t CalcMonthBeginTime(time_t cur_time);
    // ��ȡ�¸���0��ʱ�䣬��λ��
    static time_t GetNextMonthBeginTime();
    static time_t CalcNextMonthBeginTime(time_t cur_time);

	/* ��ʽ��ʱ��
	 * @param time: the time to format
	 * @return YMD: YYYY-MM-DD
	 * @param ymd_split �������и��ַ�
	 * @return HMS: HH:MM:SS
	 * @param hms_split ʱ�����и��ַ�
	 */
	static void FormatTime(time_t time, std::string* YMD, char ymd_split, std::string* HMS, char hms_split);
	static std::string FormatTime(time_t time);
};

} // ALittle

#endif // _ALITTLE_TIMEHELPER_H_
