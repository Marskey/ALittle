﻿
#ifndef _ALITTLE_ALITTLESCRIPTOPNEWSTATREFERENCE_H_
#define _ALITTLE_ALITTLESCRIPTOPNEWSTATREFERENCE_H_

#include "ALittleScriptReferenceTemplate.h"

#include "../Generate/ALittleScriptOpNewStatElement.h"

class ALittleScriptOpNewStatReference : public ALittleScriptReferenceTemplate<ALittleScriptOpNewStatElement>
{
public:
    ALittleScriptOpNewStatReference(ABnfElementPtr element) : ALittleScriptReferenceTemplate<ALittleScriptOpNewStatElement>(element) {}
    ABnfGuessError GuessTypes(std::vector<ABnfGuessPtr>& guess_list) override;

    ABnfGuessError CheckError() override;
};

#endif // _ALITTLE_ALITTLESCRIPTOPNEWSTATREFERENCE_H_