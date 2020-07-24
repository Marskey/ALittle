#include "ALittleScriptIfExprReference.h"

#include "../Index/ALittleScriptUtility.h"
#include "../Generate/ALittleScriptValueStatElement.h"
#include "../Guess/ALittleScriptGuessPrimitive.h"

ABnfGuessError ALittleScriptIfExprReference::CheckError()
{
    auto element = m_element.lock();
    if (element == nullptr) return ABnfGuessError(element, u8"�ڵ�ʧЧ");
    if (element->GetAllExpr() == nullptr && element->GetIfBody() == nullptr)
        return ABnfGuessError(element, u8"ifû�б���ʽ�б�");
    return nullptr;
}