﻿#ifndef _ALITTLE_ALITTLESCRIPTTCALLSTATREFERENCE_H_
#define _ALITTLE_ALITTLESCRIPTTCALLSTATREFERENCE_H_

#include "ALittleScriptReferenceTemplate.h"

#include "../Generate/ALittleScriptTcallStatElement.h"

class ALittleScriptTcallStatReference : public ALittleScriptReferenceTemplate<ALittleScriptTcallStatElement>
{
public:
    ALittleScriptTcallStatReference(ABnfElementPtr element) : ALittleScriptReferenceTemplate<ALittleScriptTcallStatElement>(element) {}
    ABnfGuessError GuessTypes(std::vector<ABnfGuessPtr>& guess_list) override;

    ABnfGuessError CheckError() override;
};

#endif // _ALITTLE_ALITTLESCRIPTTCALLSTATREFERENCE_H_