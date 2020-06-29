
#ifndef _ALITTLE_ABNFFILECLASS_H_
#define _ALITTLE_ABNFFILECLASS_H_

#include "ALittle/LibCommon/ThirdParty/alanguage/Index/ABnfFile.h"
#include <unordered_map>
#include <unordered_set>
#include <string>

class ABnfFileClass : public ABnfFile
{
private:
    // ������ӳ���������
    std::unordered_map<std::string, std::unordered_set<ABnfNodeElementPtr>> m_rule;
    // ��ȡ����
    std::unordered_map<std::string, std::unordered_set<ABnfNodeElementPtr>> m_index;

public:
	ABnfFileClass(ABnfProject* project, const std::string& full_path, ABnf* abnf, const char* text, size_t len);
    virtual ~ABnfFileClass();

    inline const std::unordered_map<std::string, std::unordered_set<ABnfNodeElementPtr>>& GetRuleSet() { return m_rule; }
    inline const std::unordered_map<std::string, std::unordered_set<ABnfNodeElementPtr>>& GetIndex() { return m_index; }

    // ���·�������
    void UpdateAnalysis() override;
    // ���´�����Ϣ
    void UpdateError() override;
    // �ռ�����
    void CollectIndex(ABnfNodeElementPtr node);
    // �ռ�����ID
    void CollectRule();
};

#endif // _ALITTLE_ABNFFILECLASS_H_
