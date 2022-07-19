#pragma once
#include "EnochFreeLancerAct.h"
#include "../SkillData.h"

class FLRangeSkill :
    public FreeLancerAct
{
public:
	void _End();
	SkillData skillData;
};

