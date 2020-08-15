
#ifndef _ALITTLE_HTTPHELPER_H_
#define _ALITTLE_HTTPHELPER_H_

#include <string>
#include <map>

namespace ALittle
{

class HttpHelper
{
public:
	enum class ResponseType
	{
		RESPONSE_TYPE_CONTENT_LENGTH,
		RESPONSE_TYPE_CHUNK,
		RESPONSE_TYPE_DATAFOLLOW
	};

	// ��ʮ��������ת����ֵ��������ַǷ��ַ��ͷ���false
	static bool String2HexNumber(int& result, const std::string& content);

public:
	// url����
	static std::string UrlEncode(const std::string& url);
	// PHP�汾��url����
	static std::string PHPUrlEncode(const std::string& url);
	// url����
	static std::string UrlDecode(const std::string& url);
	
public:
	// ����Get����ͷ
	static void GenerateGetRequestHead(const std::string& domain, const std::string& path, std::string& result);
	// ����Post����ͷ
	static void GeneratePostRequestHead(const std::string& domain, const std::string& path, const std::string& content, std::string& result);
	
public:
	static bool AnalysisRequest(const std::string& request, std::string& method, std::string& path, std::string* param, std::string* content_type, std::string* content);
	
public:
	// ��httpͷ��ȡstatus
	static bool CalcStatusFromHttp(const std::string& response, std::string& status);
	// ��httpͷ��ȡ�ļ���С
	static bool CalcFileSizeFromHttp(const std::string& response, int& length, ResponseType& type);
	// ��httpͷ��ȡ�����Ͷ˿�
	static bool CalcDomainAndPortByUrl(const std::string& url, std::string& demain, int& port, std::string& path);
	// ��httpͷ��ȡ��ȡ�ļ�����
	static bool CalcContentTypeFromHttp(const std::string& response, std::string& type);
	
public:
	// ���ݺ�׺�����Ҷ�Ӧ���ļ��ļ�����
    static std::string GetContentTypeByExt(const std::string& ext);
};

} // ALittle

#endif // _ALITTLE_HTTPHELPER_H_
