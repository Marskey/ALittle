
#ifndef _ALITTLE_ABNFREFERENCE_H_
#define _ALITTLE_ABNFREFERENCE_H_

#include "ABnfGuess.h"
#include "ALanguageHelperInfo.h"

#include <memory>

class ABnfReference
{
public:
    ABnfReference() { }
    virtual ~ABnfReference() {}

    // ������
    virtual void CheckError(ABnfGuessError& error) { }

    // ��ȡ����
    virtual void GuessTypes(std::vector<ABnfGuess>& guess_list, ABnfGuessError& error) { }

    // ���ض�����ʽ������
    virtual bool MultiGuessTypes() { return false; }

    // ��ȡ����
    virtual int GetDesiredIndentation(int offset, ABnfElementPtr select) { return 0; }

    // ��ȡ����
    virtual int GetFormatIndentation(int offset, ABnfElementPtr select) { return 0; }

    // ��������ʱ�ĺ�����ʾ
    virtual void QuerySignatureHelp(int& start, int& length, ALanguageSignatureInfo& info)
    {
        start = 0;
        length = 0;
    }

    // �������ʱ����ʾ�Ŀ����Ϣ
    virtual void QueryQuickInfo(std::string& info) { }

    // �������ܲ�ȫ
    virtual bool QueryCompletion(int offset, std::vector<ALanguageCompletionInfo>& list) { return false; }

    // ��ɫ
    virtual void QueryClassificationTag(bool& blur, std::string& tag) { blur = false; }

    // ����ʰȡ
    virtual bool PeekHighlightWord() { return false; }

    // ���и���
    virtual void QueryHighlightWordTag(std::vector<ALanguageHighlightWordInfo>& list) { }

    // ��ת
    virtual ABnfElementPtr GotoDefinition() { return nullptr; }

    // �Ƿ������ת
    virtual bool CanGotoDefinition() { return true; }
};

template <typename T>
class ABnfReferenceTemplate : public ABnfReference
{
protected:
    std::shared_ptr<T> m_element;

public:
    ABnfReferenceTemplate(ABnfElementPtr element) { m_element = std::dynamic_pointer_cast<T>(element); }
};

#endif // _ALITTLE_ABNFREFERENCE_H_