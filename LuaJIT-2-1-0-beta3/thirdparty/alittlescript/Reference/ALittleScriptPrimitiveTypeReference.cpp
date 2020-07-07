#include "ALittleScriptPrimitiveTypeReference.h"

#include "../Index/ALittleScriptIndex.h"

ABnfGuessError ALittleScriptPrimitiveTypeReference::GuessTypes(std::vector<ABnfGuessPtr>& guess_list)
{
    auto element = m_element.lock();
    if (element == nullptr) return ABnfGuessError(nullptr, u8"�ڵ�ʧЧ");
    auto* index = GetIndex();
    if (index == nullptr) return  ABnfGuessError(nullptr, u8"���ڹ�����");
    auto it = index->sPrimitiveGuessListMap.find(element->GetElementText());
    if (it != index->sPrimitiveGuessListMap.end())
    {
        guess_list = it->second;
        return nullptr;
    }
    guess_list.resize(0);
    return nullptr;
}
