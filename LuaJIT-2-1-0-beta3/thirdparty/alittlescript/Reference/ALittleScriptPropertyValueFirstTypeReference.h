﻿
#ifndef _ALITTLE_ALITTLESCRIPTPROPERTYVALUEFIRSTTYPEREFERENCE_H_
#define _ALITTLE_ALITTLESCRIPTPROPERTYVALUEFIRSTTYPEREFERENCE_H_

#include "ALittleScriptReferenceTemplate.h"

#include "../Generate/ALittleScriptPropertyValueFirstTypeElement.h"

class ALittleScriptPropertyValueFirstTypeReference : public ALittleScriptReferenceTemplate<ALittleScriptPropertyValueFirstTypeElement>
{
    ABnfGuessError GuessTypes(std::vector<ABnfGuessPtr>& guess_list) override;
};

#endif // _ALITTLE_ALITTLESCRIPTPROPERTYVALUEFIRSTTYPEREFERENCE_H_