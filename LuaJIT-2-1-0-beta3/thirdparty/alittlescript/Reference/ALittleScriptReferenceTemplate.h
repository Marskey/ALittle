
#ifndef _ALITTLE_ALITTLESCRIPTREFERENCETEMPLATE_H_
#define _ALITTLE_ALITTLESCRIPTREFERENCETEMPLATE_H_

#include "../../alanguage/Model/ABnfReference.h"

#include <memory>

class ALittleScriptReference : public ABnfReference
{
private:
    int m_format_indent = -1;
    int m_desire_indent = -1;
    ABnfElementWeakPtr m_ref_element;

public:
    ALittleScriptReference(ABnfElementPtr p_element);
    virtual ~ALittleScriptReference() {}

    // �������ʱ����ʾ�Ŀ����Ϣ
    virtual void QueryQuickInfo(std::string& info) override;

    virtual int QueryDesiredIndent(int it_line, int it_char, ABnfElementPtr select) override;

    virtual int QueryFormateIndent(int it_line, int it_char, ABnfElementPtr select) override;
};

template <typename T>
class ALittleScriptReferenceTemplate : public ALittleScriptReference
{
protected:
    std::weak_ptr<T> m_element;

public:
    ALittleScriptReferenceTemplate(ABnfElementPtr p_element) : ALittleScriptReference(p_element) { m_element = std::dynamic_pointer_cast<T>(p_element); }
    virtual ~ALittleScriptReferenceTemplate() {}
};

#endif // _ALITTLE_ALITTLESCRIPTREFERENCETEMPLATE_H_