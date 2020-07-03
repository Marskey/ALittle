
#ifndef _ALITTLE_ALITTLESCRIPTSTRUCTDATA_H_
#define _ALITTLE_ALITTLESCRIPTSTRUCTDATA_H_

#include <unordered_map>
#include <set>
#include <memory>

class ALittleScriptStructVarDecElement;

class ALittleScriptStructData
{
private:
    // Key:���ƣ�Value:��Ա
    std::unordered_map<std::string, std::shared_ptr<ALittleScriptStructVarDecElement>> m_element_map;

public:
    // �����Ԫ��
    void AddVarDec(std::shared_ptr<ALittleScriptStructVarDecElement> dec);

    // ����Ԫ��
    void FindVarDecList(const std::string& name, std::vector<std::shared_ptr<ALittleScriptStructVarDecElement>>& result);
};

#endif // _ALITTLE_ALITTLESCRIPTSTRUCTDATA_H_
