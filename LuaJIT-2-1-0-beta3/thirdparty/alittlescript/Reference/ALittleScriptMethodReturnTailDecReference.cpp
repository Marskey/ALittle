#include "ALittleScriptMethodReturnTailDecReference.h"

#include "../Guess/ALittleScriptGuessReturnTail.h"
#include "../Index/ALittleScriptFileClass.h"

ABnfGuessError ALittleScriptMethodReturnTailDecReference::GuessTypes(std::vector<ABnfGuessPtr>& guess_list)
{
    auto element = m_element.lock();
    if (element == nullptr) return ABnfGuessError(nullptr, u8"�ڵ�ʧЧ");
    auto info = ABnfGuessPtr(new ALittleScriptGuessReturnTail(element->GetElementText()));
    info->UpdateValue();
    element->GetFile()->AddGuessType(info);
    guess_list.push_back(info);
    return nullptr;
}
