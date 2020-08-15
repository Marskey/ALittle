
#ifndef _ALITTLE_FILECACHESYSTEM_H_
#define _ALITTLE_FILECACHESYSTEM_H_

#include <vector>
#include <map>
#include <memory>
#include <string>

namespace ALittle
{

class FileCache
{
public:
	FileCache(int unit_size);
	~FileCache();

public:
	enum Status
	{
		FILECACHESTATUS_IDLE,           // ����
		FILECACHESTATUS_LOADING,        // ������
		FILECACHESTATUS_LOADED,         // �������
	};

public:
	// ��ȡ״̬
	Status GetStatus() const;

    // ����״̬
	void SetStatus(Status status);

    // ��ȡ��С
    int GetSize() const;

public:
    // ������д�뻺��
    void Write(void* memory, int size);
    // �ӻ����ܶ�ȡ����
	int Read(int offset, void* buffer, int size);

    // ������
	void Clear();

private:
	Status m_status;					// ����״̬
	int m_unit_size;					// �ڴ���С

	int m_last_empty_size;				// ���һ������ڴ��С
	int m_total_size;					// �ܴ�С
	std::vector<void*> m_memory_list;	// �ڴ���б�
};

typedef std::shared_ptr<FileCache> FileCachePtr;
class FileCacheSystem;

class FileCacheHelper
{
public:
	FileCacheHelper(FileCacheSystem* system);
	~FileCacheHelper();

public:
    // ���ļ�
	bool Open(const std::string& file_path, bool use_cache);
	bool IsOpen();

    // �ر��ļ�
	void Close();
	
public:
    // ��ȡ�ļ���С
	int GetFileSize();
    // ����ƫ��
	void SetOffset(int offset);
    // ��ȡ�ļ�
	int Read(void* buffer, int size);

private:
	FileCachePtr m_file_cache;  // �ļ�����
	int m_offset;               // ƫ��
	
private:
	FILE* m_native_file;        // ԭʼ�ļ�
	int m_native_size;          // ԭʼ�ļ���С

private:
	FileCacheSystem* m_system;
};

class FileCacheSystem
{
public:
	FileCacheSystem();
	~FileCacheSystem();

public:
	void Setup();
	void Shutdown();

public:
    // �����ļ���������
	void SetMaxSize(int max_size);

public:
    // ������ʹ�õ�ʱ��С��time
    // ��ô�������
	void ClearByTime(int time);
	
    // ���ĳЩ�ļ�����size�Ĵ�С
    // ��ô�������
	void ClearBySize(int size);
	
    // �����ǰ�����ļ��Ĵ�С�ܺͳ���max_size
    // ��ô������max_sizeΪֹ
	void ClearByMaxSize(int max_size);
	
    // �����ļ�·��������
	void ClearByPath(const char* file_path);
	
    // ����ȫ��
	void ClearAll();

public:
	bool Create(const std::string& file_path, FileCachePtr& file_cache, FILE** native_file, int& native_size);
	
private:
	struct FileCacheInfo
	{
		FileCachePtr file_cache;    // ����
		time_t create_time;         // ����ʱ��
		time_t update_time;         // ����ʱ��
		int native_size;            // �ļ���С
	};

	typedef std::map<std::string, FileCacheInfo> FileCacheInfoMap;
	FileCacheInfoMap m_file_cache_map;

	int m_total_size;
	int m_max_size;
};

} // ALittle

#endif // _ALITTLE_FILECACHESYSTEM_H_
