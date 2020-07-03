#ifndef _ALITTLE_ALittleScriptEmptyExprElement_H_
#define _ALITTLE_ALittleScriptEmptyExprElement_H_

#include <memory>
#include <vector>
#include <string>
#include "../../alanguage/Model/ABnfNodeElement.h"

class ALittleScriptStringElement;

class ALittleScriptEmptyExprElement : public ABnfNodeElement
{
public:
    ALittleScriptEmptyExprElement(ABnfFactory* factory, ABnfFile* file, int line, int col, int offset, const std::string& type)
        : ABnfNodeElement(factory, file, line, col, offset, type) { }
    virtual ~ALittleScriptEmptyExprElement() { }

private:
    bool m_flag_String = false;
    std::shared_ptr<ALittleScriptStringElement> m_cache_String;
public:
    std::shared_ptr<ALittleScriptStringElement> GetString()
    {
        if (m_flag_String) return m_cache_String;
        m_flag_String = true;
        for (auto& child : m_childs)
        {
            auto node = std::dynamic_pointer_cast<ALittleScriptStringElement>(child);
            if (node != nullptr)
            {
                m_cache_String = node;
                break;
            }
        }
        return m_cache_String;
    }
};

#endif // _ALITTLE_ALittleScriptEmptyExprElement_H_
