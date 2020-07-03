
#ifndef _ALITTLE_ALITTLESCRIPTACCESSDATA_H_
#define _ALITTLE_ALITTLESCRIPTACCESSDATA_H_

#include <unordered_map>
#include <set>
#include <memory>

class ABnfElement;
enum ABnfElementType;

class ALittleScriptAccessData
{
private:
	// Key1:Ԫ�����ͣ�Key2:���ƣ�Value:��Ӧ��Ԫ�ؼ���
	std::unordered_map<ABnfElementType, std::unordered_map<std::string, std::set<std::shared_ptr<ABnfElement>>>> m_element_map;

public:
    // ��ȡ����
    const std::unordered_map<ABnfElementType, std::unordered_map<std::string, std::set<std::shared_ptr<ABnfElement>>>>& GetElementMap() { return m_element_map; }

    // �����Ԫ��
    void AddNameDec(std::shared_ptr<ABnfElement> dec);

    // ����Ԫ��
    void FindNameDecList(ABnfElementType type, const std::string& name, std::vector<std::shared_ptr<ABnfElement>>& result);

    // �Ƴ�Ԫ��
    void RemoveNameDec(std::shared_ptr<ABnfElement> dec);
};

#endif // _ALITTLE_ALITTLESCRIPTACCESSDATA_H_
