#include "FLRangeAttack.h"
#include "../EnochFreeLancerData.h"
#include "../EnochActorDataFactory.h"
#include "../EnochProjectileData.h"
#include "../EnochSimulator.h"


void FLRangeAttack::_End()
{
	auto targetData = data->GetTarget();
	if (targetData == nullptr) return;
	data->direction = targetData->locationOnFight - data->locationOnFight;
	auto factory = EnochActorDataFactory::instance;
	auto SN_PJ = factory->SpawnProjectile();
	//if (SN_PJ < 0) 오류상황 처리안됨
	auto projectileData = factory->GetProjectileData(SN_PJ);
	if (projectileData != nullptr) {
		auto simulator = EnochSimulator::instance;
		simulator->ProjectileQueue->push_back(SN_PJ);
		projectileData->Set(data->attackDamage, 5.0f, targetData->GetSerialNumber(), data->GetSerialNumber());
		EnochSimulator::logs.push({ SimulateInfoType::RangeAttack, SN_PJ, targetData->GetSerialNumber(),SN_PJ });
	}
	data->SetState(FreeLancerState::PostAttack);
	time = 0;
	
}