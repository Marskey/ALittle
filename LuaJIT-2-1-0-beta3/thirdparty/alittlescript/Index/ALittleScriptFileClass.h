
#ifndef _ALITTLE_ALITTLESCRIPTFILECLASS_H_
#define _ALITTLE_ALITTLESCRIPTFILECLASS_H_

#include "ALittle/LibCommon/ThirdParty/alanguage/Index/ABnfFile.h"
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <string>

class ALittleScriptFileClass : public ABnfFile
{
public:
    ALittleScriptFileClass(ABnfProject* project, const std::string& full_path, ABnf* abnf, const char* text, size_t len);
    virtual ~ALittleScriptFileClass();

    // ���·�������
    void UpdateAnalysis() override;
    // ���´�����Ϣ
    void UpdateError() override;
};

#endif // _ALITTLE_ALITTLESCRIPTFILECLASS_H_
