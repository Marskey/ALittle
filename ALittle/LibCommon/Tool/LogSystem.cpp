
#include "LogSystem.h"

#include "ALittle/LibCommon/Helper/LogHelper.h"
#include "ALittle/LibCommon/Helper/FileHelper.h"
#include "ALittle/LibCommon/Helper/TimeHelper.h"
#include "ALittle/LibCommon/Helper/StringHelper.h"
#include "ALittle/LibCommon/ScriptSystem/ScriptSystem.h"

#ifdef __ANDROID__
#include <android/log.h>
#endif

namespace ALittle
{

LogSystem::LogSystem()
: m_current_day(0), m_file(nullptr)
#ifdef _WIN32
, m_out(NULL)
#endif
{
}

LogSystem::~LogSystem()
{
	// �ͷ�
	Shutdown();
}

LogSystem& LogSystem::Instance()
{
	static LogSystem instance;
	return instance;
}

void LogSystem::Setup(const std::string& path, const std::string& name)
{
	// �����ļ���
	m_file_name = name;
	m_file_path = path;

	// �����ļ���
	FileHelper::CreateDeepFolder(m_file_path);

#ifdef _WIN32
	// ��ȡ����̨���
	m_out = GetStdHandle(STD_OUTPUT_HANDLE);
#endif // _WIN32

	// ����
	Start();
}

void LogSystem::Log(const char* content, short level)
{
	// �����δ����ֱ�ӷ���
	if (!IsStart())
	{
		printf("%s\n", content);
		return;
	}

	// �ȴ��ڴ���л�ȡ�������û�оʹ���һ��
	LogStringInfo log;
	// ������ɫ
	log.level = level;
	// ���õ�ǰʱ��
	TimeHelper::FormatTime(TimeHelper::GetCurTime(), 0, '-', &(log.content), ':');
	// ���һ���ո�
	log.content.push_back(' ');
	// �����־����
	log.content.append(content);
	// ��ӻ���
	log.content.push_back('\n');
	// ����־��ӵ��б�
    Add(log);
}

void LogSystem::Flush(LogStringInfo& info)
{
	// ��ȡ���ڵ�ʱ��
	time_t current_time = TimeHelper::GetCurTime();
	// �����ǲ��ǵڶ�����
	if (current_time - m_current_day >= 60*60*24)
	{
		if (m_file != nullptr)
        {
			fclose(m_file);
			m_file = nullptr;
		}

        std::string YMD, HMS;
        TimeHelper::FormatTime(current_time, &YMD, '-', &HMS, '-');

		// ����һ���µ��ļ�����
		std::string file_path;
		file_path.append(m_file_path).append(m_file_name).append("_").append(YMD).append("_").append(HMS).append(".log");
#ifdef _WIN32
		fopen_s(&m_file, file_path.c_str(), "a");
#else
		m_file = fopen(file_path.c_str(), "a");
#endif

        if (m_file == nullptr)
            printf("log file open failed:%s", file_path.c_str());

		// ������ʱ���0�㱣������
		m_current_day = TimeHelper::CalcTodayBeginTime(current_time);
	}

    if (m_file == nullptr)
        return;

	// ���ÿ���̨��ɫ
#ifdef _WIN32
	if (info.level == LogHelper::LL_INFO)
		SetConsoleTextAttribute(m_out, LogHelper::LC_INFO);
	else if (info.level == LogHelper::LL_WARN)
		SetConsoleTextAttribute(m_out, LogHelper::LC_WARN);
	else if (info.level == LogHelper::LL_ERROR)
		SetConsoleTextAttribute(m_out, LogHelper::LC_ERROR);
	else if (info.level == LogHelper::LL_SYSTEM)
		SetConsoleTextAttribute(m_out, LogHelper::LC_SYSTEM);
	else if (info.level == LogHelper::LL_DATABASE)
		SetConsoleTextAttribute(m_out, LogHelper::LC_DATABASE);
	else if (info.level == LogHelper::LL_EVENT)
		SetConsoleTextAttribute(m_out, LogHelper::LC_EVENT);
	else
		SetConsoleTextAttribute(m_out, LogHelper::LC_INFO);
    printf("%s", StringHelper::UTF82ANSI(info.content.c_str()).c_str());
    SetConsoleTextAttribute(m_out, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#elif __ANDROID__
	if (info.color == LogHelper::LL_ERROR)
		__android_log_write(ANDROID_LOG_ERROR, "SDL", info.content.c_str());
	else if (info.color == LogHelper::LL_WARN)
		__android_log_write(ANDROID_LOG_WARN, "SDL", info.content.c_str());
	else
		__android_log_write(ANDROID_LOG_INFO, "SDL", info.content.c_str());
#else
	printf("%s", info.content.c_str());
#endif
	// д�뵽�ļ�
	std::fwrite(info.content.data(), 1, info.content.size(), m_file);
}

void LogSystem::Shutdown()
{
    // �ر�
    Stop();

	// �ر��ļ�
	if (m_file)
	{
		fclose(m_file);
		m_file = nullptr;
	}
}

} // ALittle
