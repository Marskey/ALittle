
#ifndef _ALITTLE_ALITTLESCRIPTCLASSDATA_H_
#define _ALITTLE_ALITTLESCRIPTCLASSDATA_H_

#include <unordered_map>
#include <set>
#include <memory>

class ABnfElement;
enum ClassAccessType;
enum ClassAttrType;

class ALittleScriptClassData
{
private:
    // Key1:����Ȩ�ޣ�Key2:�������ͣ�Key3:���ƣ�Value:Ԫ��
    std::unordered_map<ClassAccessType, std::unordered_map<ClassAttrType, std::unordered_map<std::string, std::shared_ptr<ABnfElement>>>> m_element_map;

    // ��ȡԪ�ؼ���    
    const std::unordered_map<std::string, std::shared_ptr<ABnfElement>>* GetElementMap(ClassAttrType attr_type, ClassAccessType access_type);

public:
    // �����Ԫ��
    void AddClassChildDec(std::shared_ptr<ABnfElement> dec);

    // ����Ԫ��
    void FindClassAttrList(int access_level, ClassAttrType attr_type, const std::string& name, std::vector<std::shared_ptr<ABnfElement>>& result) const;
};

#endif // _ALITTLE_ALITTLESCRIPTCLASSDATA_H_
