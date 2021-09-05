// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFreeLancerAct.h"
#include "EnochActorDataFactory.h"
#include "EnochFreeLancerState.h"
#include "EnochFreeLancerData.h"
#include "EnochFieldData.h"
#include "EnochFieldCellData.h"
#include "EnochSimulator.h"

using namespace std;

unique_ptr<vector<function<bool(EnochFreeLancerData*)>>> EnochFreeLancerAct::functionMap;
EnochSimulator* EnochFreeLancerAct::simulator;

bool EnochFreeLancerAct::Act(EnochFreeLancerData* freeLancer)
{
	if(freeLancer->GetState() != FreeLancerState::None && freeLancer->GetState() != FreeLancerState::Skill)
		return functionMap->at((int)freeLancer->GetState())(freeLancer);
	return false;
}

void EnochFreeLancerAct::Init(EnochSimulator *simulatorIn)
{
	simulator = simulatorIn;
	functionMap = make_unique<vector<function<bool(EnochFreeLancerData*)>>>();
	functionMap->resize((int)FreeLancerState::Dead + 1);

	//Idle State
	functionMap->at((int)FreeLancerState::Idle) = [](EnochFreeLancerData* freeLancer) {
		
		return true; 
	};

	//Dead State
	functionMap->at((int)FreeLancerState::Dead) = [](EnochFreeLancerData* freeLancer) {return false; };

	//PreMove State
	functionMap->at((int)FreeLancerState::PreMove) = [](EnochFreeLancerData* freeLancer)
	{
		auto currentFieldCell = EnochFieldData::GetData(freeLancer->moveSrcLocation);
		auto targetFieldCell = EnochFieldData::GetData(freeLancer->moveDstLocation);
		if (!currentFieldCell || !targetFieldCell) {
			return false;
		}
		if (targetFieldCell->FreeLancerOnFight) {
			freeLancer->SetState(FreeLancerState::Idle);
			return true;
		}
		freeLancer->direction = freeLancer->moveDstLocation - freeLancer->moveSrcLocation;
		currentFieldCell->SetFreeLancerOnFight(nullptr);
		targetFieldCell->SetFreeLancerOnFight(freeLancer);
		freeLancer->locationOnFight = targetFieldCell->location;
		freeLancer->SetState(FreeLancerState::PostMove);
		return false;
	};

	//PostMove State
	functionMap->at((int)FreeLancerState::PostMove) = [](EnochFreeLancerData* freeLancer)
	{
		freeLancer->direction = freeLancer->moveDstLocation - freeLancer->moveSrcLocation;
		freeLancer->SetState(FreeLancerState::Idle);
		//타깃이 죽은 경우 변경하기위해 널 대입
		if (freeLancer->target && freeLancer->target->GetState() == FreeLancerState::Dead)
			freeLancer->target = nullptr;
		return true;
	};

	//PreAttack State
	functionMap->at((int)FreeLancerState::PreAttack) = [](EnochFreeLancerData* freeLancer)
	{
		freeLancer->direction = freeLancer->target->locationOnFight - freeLancer->locationOnFight;
		if (freeLancer->isMelee)
			EnochFreeLancerData::DoAttack(freeLancer, freeLancer->target);
		else {
			//Todo : 투사체 속력이 추가되어야함. 투사체 속력에 따른 도착시간 계산이 추가되어야함. 현재는 둘다 1.0으로 설정됨
			auto factory = EnochActorDataFactory::instance;
			auto SpawnIndex = factory->SpawnProjectile();
			if (SpawnIndex < 0) return false; // 오류상황
			auto projectileData = factory->GetProjectileData(SpawnIndex);
			if (projectileData != nullptr) {
				projectileData->Set(freeLancer->attackDamage, 1.0f, freeLancer->target, freeLancer);
				simulator->ProjectileQueue->push({ projectileData,1.0f });
			}
		}
		freeLancer->SetState(FreeLancerState::PostAttack);
		return false;
	};

	//PostAttack State
	functionMap->at((int)FreeLancerState::PostAttack) = [](EnochFreeLancerData* freeLancer)
	{
		if (!freeLancer->target || freeLancer->target->GetState() == FreeLancerState::Dead) {
			freeLancer->SetState(FreeLancerState::Idle);
		}
		return true;
	};
}