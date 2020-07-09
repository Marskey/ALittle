
#ifndef _ALITTLE_AREGEX_H_
#define _ALITTLE_AREGEX_H_

#include <string>
#include <vector>
#include <unordered_set>

enum class ARegexType
{
    T_NONE,            // ��Чö��
    T_CHAR,            // һ���ַ�
    T_CHARSET,         // �ַ�����
    T_LIST,           // �ӽڵ��б� ���ƥ��
    T_OPTION,           // �ӽڵ��б� ��֧ƥ��
};

enum class ARegexRepeatType
{
    NRT_NONE,		    // δ���ã������Ե�NRT_ONE����
    NRT_ONE,	        // ���ҽ���һ��
    NRT_NOT_OR_MORE,    // 0�����߶��
    NRT_AT_LEAST_ONE,   // ����һ��
    NRT_ONE_OR_NOT,	    // ���ҽ���һ�� ����û��
};

enum class ARegexCharSetType
{
    CST_CUSTOM, // �Զ��壬��ȡchar_set
    CST_A,      // ��ĸ
    CST_D,      // ����
    CST_W,      // ��ĸ�����»���
    CST_DOT,      // ����\n������ַ�
    CST_ALL,      // ȫ���ַ�
};

enum class ARegexNegateType
{
    NNT_NONE,		    // δ���ã������Ե�NNT_ONE����
    NNT_FALSE,	        // û��ȡ��
    NNT_TRUE,	        // ��ȡ��
};

struct ARegexNode
{
    ARegexType type = ARegexType::T_NONE;        // �ڵ�����

    // CHAR
    char value = 0;     // �ַ�����

    // CHARSET
    std::unordered_set<char> char_set;// �ַ�����
    ARegexCharSetType char_set_type = ARegexCharSetType::CST_CUSTOM;
    ARegexNegateType negate = ARegexNegateType::NNT_NONE;   // �Ƿ���ȡ��

    // GROUP
    std::vector<ARegexNode> childs; // �ӽڵ�

    ARegexRepeatType repeat = ARegexRepeatType::NRT_NONE;
};

class ARegex
{
private:
	std::string m_rule;	// ����
    std::string m_rule_error;    // ��¼����������

    // �ѱ���õĹ�������
    ARegexNode m_node;

public:
	ARegex(const std::string& rule);
    // �Ӹ�λ�ý���ƥ��
    bool Match(const char* src, int& length);

private:
    bool MatchNode(const char* src, const ARegexNode& node, int& length);
    bool MatchNodeRepeated(const char* src, const ARegexNode& node, int& length);

private:
    static bool CompileOption(const std::string& rule, size_t& offset, ARegexNode& node, std::string& error);
    static bool CompileCustomSet(const std::string& rule, size_t& offset, ARegexNode& node, std::string& error);
};

#endif // _ALITTLE_AREGEX_H_