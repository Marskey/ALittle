
#ifndef _ALITTLE_FILEHELPER_H_
#define _ALITTLE_FILEHELPER_H_

#include <string>
#include <vector>

struct SDL_RWops;

namespace ALittle
{

class FileHelper
{
public:
    // �����ļ���
	static void CreateFolder(const std::string& path);

    // �ݹ鴴���ļ���
	static void CreateDeepFolder(const std::string& path);

    // �ж��ļ��Ƿ����
	static bool IsFileExist(const std::string& path);
	
    // ��·���л�ȡ�ļ���չ��
	static std::string GetFileExtByPath(const std::string& file_path);

	// ��·���л�ȡ�ļ���
	static std::string GetFileNameByPath(const std::string& file_path);

    // ��·���л�ȡ�ļ���(������׺)
    static std::string GetJustFileNameByPath(const std::string& file_path);

	// Ϊ�ļ���β���·���ָ���
	static std::string TryAddFileSeparator(const std::string& file_path);

	// �����ļ�
	static bool LoadStdFile(const std::string& file_path, std::vector<char>& out);
};

} // ALittle

#endif // _ALITTLE_FILEHELPER_H_
