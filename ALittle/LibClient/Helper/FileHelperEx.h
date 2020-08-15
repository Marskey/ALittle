
#ifndef _ALITTLE_FILEHELPEREX_H_
#define _ALITTLE_FILEHELPEREX_H_

#include <string>
#include <vector>
#include <set>

struct SDL_RWops;

namespace ALittle
{

class FileHelperEx
{
public:
	// ��ȡ����·��
	static std::string BaseFilePath();

	// ��ȡ�ⲿ·��
	static std::string ExternalFilePath();

	// �����ļ���md5
	static std::string FileMD5(const std::string& file_path);

	// ���ļ�
	static SDL_RWops* OpenFile(const std::string& path, bool only_from_asset, const char* mode);

	// �����ļ�
	static bool CpFile(const char* src_path, const char* dest_path, bool only_from_asset);

	// �����ļ�
	static bool LoadFile(const std::string& path, bool only_from_asset, std::vector<char>& memory);

	// ���浽�ļ�
	static bool SaveFile(const char* target_path, const char* content, int size);
};

} // ALittle

#endif // _ALITTLE_FILEHELPEREX_H_
