#ifndef _ALITTLE_ALittleScriptLineCommentElement_H_
#define _ALITTLE_ALittleScriptLineCommentElement_H_

#include <memory>
#include <vector>
#include <string>
#include "../../alanguage/Model/ABnfNodeElement.h"

class ALittleScriptRegexElement;

class ALittleScriptLineCommentElement : public ABnfNodeElement
{
public:
    ALittleScriptLineCommentElement(ABnfFactory* factory, ABnfFile* file, int line, int col, int offset, const std::string& type)
        : ABnfNodeElement(factory, file, line, col, offset, type) { }
    virtual ~ALittleScriptLineCommentElement() { }

private:
    bool m_flag_Regex = false;
    std::shared_ptr<ALittleScriptRegexElement> m_cache_Regex;
public:
    std::shared_ptr<ALittleScriptRegexElement> GetRegex()
    {
        if (m_flag_Regex) return m_cache_Regex;
        m_flag_Regex = true;
        for (auto& child : m_childs)
        {
            auto node = std::dynamic_pointer_cast<ALittleScriptRegexElement>(child);
            if (node != nullptr)
            {
                m_cache_Regex = node;
                break;
            }
        }
        return m_cache_Regex;
    }
};

#endif // _ALITTLE_ALittleScriptLineCommentElement_H_
