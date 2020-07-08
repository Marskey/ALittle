﻿
#ifndef _ALITTLE_ALITTLESCRIPTPROPERTYVALUESUFFIXREFERENCE_H_
#define _ALITTLE_ALITTLESCRIPTPROPERTYVALUESUFFIXREFERENCE_H_

#include "ALittleScriptReferenceTemplate.h"

#include "../Generate/ALittleScriptPropertyValueSuffixElement.h"

class ALittleScriptPropertyValueSuffixReference : public ALittleScriptReferenceTemplate<ALittleScriptPropertyValueSuffixElement>
{
    ABnfGuessError GuessTypes(std::vector<ABnfGuessPtr>& guess_list) override;
};

#endif // _ALITTLE_ALITTLESCRIPTPROPERTYVALUESUFFIXREFERENCE_H_