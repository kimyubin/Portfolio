#include "FLRangeSkill.h"
#include "../EnochFreeLancerData.h"
#include "../EnochActorDataFactory.h"
#include "../EnochProjectileData.h"
#include "../EnochSimulator.h"


void FLRangeSkill::_End()
{
	if (skillData.target == SkillTarget::Enemy_Single) {
		auto targetData = data->GetTarget();
		data->direction = targetData->locationOnFight - data->locationOnFight;
		auto factory = EnochActorDataFactory::instance;
		auto SN_PJ = factory->SpawnProjectile();
		//if (SN_PJ < 0) 오류상황 처리안됨
		auto projectileData = factory->GetProjectileData(SN_PJ);
		if (projectileData != nullptr) {
			auto simulator = EnochSimulator::instance;
			simulator->ProjectileQueue->push_back(SN_PJ);
			projectileData->Set(200, 20.0f, targetData->GetSerialNumber(), data->GetSerialNumber());
			EnochSimulator::logs.push({ SimulateInfoType::Skill, SN_PJ, targetData->GetSerialNumber(),SN_PJ });
		}
		data->SetState(FreeLancerState::PostSkill);
		time = 0;
	}
}