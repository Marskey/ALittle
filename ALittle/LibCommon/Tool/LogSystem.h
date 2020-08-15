
#ifndef _ALITTLE_LOGSYSTEM_H_
#define _ALITTLE_LOGSYSTEM_H_

#include "ThreadConsumer.h"

#include <string>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

namespace ALittle
{

class ScriptSystem;

// ��־��Ϣ�ṹ��
struct LogStringInfo
{
	LogStringInfo() : level(0) {}
    std::string content;		// ��־����
    short level;				// ����
};

class LogSystem : public ThreadConsumer<LogStringInfo>
{
public:
	static LogSystem& Instance();

public:
	// ������־�ļ���ǰ׺
    void Setup(const std::string& path, const std::string& name);

private:
    // ��־�ļ�ǰ׺
    std::string m_file_name;
	std::string m_file_path;
	
public:
	// ��־
	// content ��ʾ��־������
	void Log(const char* content, short level);

	// �ر���־ϵͳ
	void Shutdown();

protected:
	// ִ����־
	virtual void Flush(LogStringInfo& info) override;

private:
    FILE* m_file;			// ��־�ļ�����
    time_t m_current_day;		// ����0���ʱ��

#ifdef _WIN32
	HANDLE m_out;		// ����̨���
#endif // _WIN32

private:
	LogSystem();
	~LogSystem();
};

} // ALittle

#define g_LogSystem ALittle::LogSystem::Instance()

#endif // _ALITTLE_LOGSYSTEM_H_
