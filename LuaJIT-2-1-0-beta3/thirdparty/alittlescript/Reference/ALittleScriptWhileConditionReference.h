﻿#ifndef _ALITTLE_ALITTLESCRIPTWHILECONDITIONREFERENCE_H_
#define _ALITTLE_ALITTLESCRIPTWHILECONDITIONREFERENCE_H_

#include "ALittleScriptReferenceTemplate.h"

#include "../Generate/ALittleScriptWhileConditionElement.h"

class ALittleScriptWhileConditionReference : public ALittleScriptReferenceTemplate<ALittleScriptWhileConditionElement>
{
public:
    ALittleScriptWhileConditionReference(ABnfElementPtr element) : ALittleScriptReferenceTemplate<ALittleScriptWhileConditionElement>(element) {}
    ABnfGuessError CheckError() override;
};

#endif // _ALITTLE_ALITTLESCRIPTWHILECONDITIONREFERENCE_H_