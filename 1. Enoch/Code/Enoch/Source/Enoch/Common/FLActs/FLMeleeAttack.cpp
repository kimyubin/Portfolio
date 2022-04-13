#include "FLMeleeAttack.h"
#include "../EnochActorDataFactory.h"
#include "../EnochFreeLancerData.h"

void FLMeleeAttack::_End()
{
	auto target = data->GetTarget();
	if (target == nullptr)
		data->direction = { -5, 0 };
	else
		data->direction = target->locationOnFight - data->locationOnFight;
	if(target != nullptr && time >= length && data->CanAttack())
		FLBattleData::DoAttack(data, target);
	data->state = FreeLancerState::None;
	data->SetState(FreeLancerState::PostAttack);
	time = 0;
}

