#ifndef _ALITTLE_ALittleScriptOp7SuffixExElement_H_
#define _ALITTLE_ALittleScriptOp7SuffixExElement_H_

#include <memory>
#include <vector>
#include <string>
#include "../../alanguage/Model/ABnfNodeElement.h"

class ALittleScriptOp7SuffixElement;
class ALittleScriptOp8SuffixElement;

class ALittleScriptOp7SuffixExElement : public ABnfNodeElement
{
public:
    ALittleScriptOp7SuffixExElement(ABnfFactory* factory, ABnfFile* file, int line, int col, int offset, const std::string& type)
        : ABnfNodeElement(factory, file, line, col, offset, type) { }
    virtual ~ALittleScriptOp7SuffixExElement() { }

private:
    bool m_flag_Op7Suffix = false;
    std::shared_ptr<ALittleScriptOp7SuffixElement> m_cache_Op7Suffix;
public:
    std::shared_ptr<ALittleScriptOp7SuffixElement> GetOp7Suffix();
private:
    bool m_flag_Op8Suffix = false;
    std::shared_ptr<ALittleScriptOp8SuffixElement> m_cache_Op8Suffix;
public:
    std::shared_ptr<ALittleScriptOp8SuffixElement> GetOp8Suffix();
};

#endif // _ALITTLE_ALittleScriptOp7SuffixExElement_H_
