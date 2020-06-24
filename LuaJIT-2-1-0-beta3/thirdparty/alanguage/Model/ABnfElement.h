
#ifndef _ALITTLE_ABNFELEMENT_H_
#define _ALITTLE_ABNFELEMENT_H_

#include <memory>
#include <string>
#include <vector>

#include "ABnfGuess.h"

class ABnfFactory;
class ABnfFile;
class ABnfReference;

class ABnfNodeElement;
using ABnfNodeElementWeakPtr = std::weak_ptr<ABnfNodeElement>;

class ABnfGuess;
using ABnfGuessPtr = std::shared_ptr<ABnfGuess>;

class ABnfElement : public std::enable_shared_from_this<ABnfElement>
{
protected:
    ABnfFactory* m_factory = nullptr;             // ���󹤳�
    ABnfFile* m_file = nullptr;           // ���ڵĽ���
    ABnfNodeElementWeakPtr m_parent;          // ���ڵ�
    ABnfReference* m_reference = nullptr;         // ����

    std::string m_element_text;         // �ı�����    
    int m_start = 0;                       // �ı�ƫ��
    int m_line = 1;                        // ������
    int m_col = 1;                         // ������

public:
    ABnfElement(ABnfFactory* factory, ABnfFile* file, int line, int col, int offset);

    virtual ~ABnfElement();

    virtual bool IsLeafOrHasChildOrError() { return false; }
    virtual bool IsError() { return false; }

    // ��ȡ����
    ABnfReference* GetReference();
    // ��ȡ����
    virtual bool GuessTypes(std::vector<ABnfGuessPtr>& guess_list, ABnfGuessError& error);

    // ��ȡ��һ������
    virtual ABnfGuessPtr GuessType(ABnfGuessError& error);

    // ��ȡ����ϸ��
    ABnfFile* GetFile() { return m_file; }
    // ��ȡ�ļ�ȫ·��
    virtual const std::string& GetFullPath();
    // ��ȡ���ڹ���·��
    virtual const std::string& GetProjectPath();

    // ���ø��ڵ�
    void SetParent(ABnfNodeElementPtr parent);
    ABnfNodeElementPtr GetParent();

    // ��ǰ�ڵ��Ƿ��ָ����Χ�н���
    bool IntersectsWith(int start, int end);

    // ����ƫ��λ�ã���ȡ������Ԫ��
    virtual ABnfElementPtr GetException(int offset) { return nullptr; }

    // ��ȡ�ڵ�ƫ��
    virtual int GetStart() { return m_start; }

    // ��ȡ�ڵ㳤��
    virtual int GetEnd() { return m_start; }

    // ��ȡ�ڵ㳤��
    virtual int GetLength() { return GetEnd() - GetStart(); }

    virtual int GetLengthWithoutError() { return GetEnd() - GetStart(); }

    // ��ȡ����
    virtual const std::string& GetNodeType();

    // ��ȡ�ı�
    virtual const std::string& GetElementText();

    // ��ȡȥ�������ŶԺ�˫���Ŷ�֮����ַ���
    virtual std::string GetElementString();

    // ��ȡ��ǰ�ǵڼ��У���1��ʼ��
    virtual int GetStartLine() { return m_line; }

    // ��ȡ��ǰ�ǵڼ��У���1��ʼ��
    virtual int GetStartCol() { return m_col; }
    // ����indent
    virtual int GetStartIndent()
    {
        int start = GetStart();
        int end = start + GetStartCol();
        int count = 0;
        for (int i = start; i < end; ++i)
        {
            if (i >= m_file->GetLength()) break;

            if (m_file->GetText()[i] == '\t')
                count += ALanguageSmartIndentProvider.s_indent_size;
            else
                ++count;
        }

        return count;
    }

    // ��ȡ����λ���ǵڼ��У���1��ʼ��
    public virtual int GetEndLine() { return m_line; }
    // ��ȡ����λ���ǵڼ��У���1��ʼ��
    public virtual int GetEndCol() { return m_col; }

    // ��ȡ������������
    public virtual void GetDesc(ref string indent, ref string result) { }

    // ��ǰ�ҵ���һ��\n���ң��м�û�пո��\t
    public virtual int FindForwardFirstEnterAndHaveNotSpaceOrTab()
    {
        int start = GetStart();
        int end = GetEnd() - 1;
        while (end >= 0 && m_file.m_text[end] != ' ' && m_file.m_text[end] != '\t' && m_file.m_text[end] != '\n')
            --end;

        for (int i = end; i >= start; --i)
        {
            if (m_file.m_text[i] == ' ' || m_file.m_text[i] == '\t')
                continue;
            if (m_file.m_text[i] == '\n')
                return i;
            break;
        }
        return -1;
    }
};

#endif // _ALITTLE_ABNFELEMENT_H_