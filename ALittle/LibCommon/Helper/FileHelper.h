
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
	// ɾ���ļ���
	static void DeleteFolder(const std::string& path);

    // �ݹ鴴���ļ���
	static void CreateDeepFolder(const std::string& path);

    // �ж��ļ��Ƿ����
	static bool IsFileExist(const std::string& path);

	// �ж��ļ����Ƿ����
	static bool IsDirExist(const std::string& path);

	// ��ȡ�ļ����µ������ļ�
	static void GetNameListInFolder(const std::string& path, std::vector<std::string>& file_list, std::vector<std::string>& dir_list);

    // ��·���л�ȡ�ļ���չ��
	static std::string GetFileExtByPath(const std::string& file_path);
	// �ı���չ��
	static std::string ChangeFileExtByPath(const std::string& file_path, const std::string& ext);

	// ��·���л�ȡ�ļ���
	static std::string GetFileNameByPath(const std::string& file_path);

	// ��·���л�ȡ�ļ���
	static std::string GetFilePathByPath(const std::string& file_path);

    // ��·���л�ȡ�ļ���(������׺)
    static std::string GetJustFileNameByPath(const std::string& file_path);

	// Ϊ�ļ���β���·���ָ���
	static std::string TryAddFileSeparator(const std::string& file_path);

	// �����ļ�
	static bool LoadStdFile(const std::string& file_path, std::vector<char>& out);
	// д���ļ�
	static bool WriteToStdFile(const std::string& file_path, const std::vector<char>& out);
};

} // ALittle

#endif // _ALITTLE_FILEHELPER_H_
