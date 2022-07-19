// Fill out your copyright notice in the Description page of Project Settings.

#include "EnochSimulator.h"
#include "EnochFiniteStateMachine.h"
#include "FLActs/EnochFreeLancerAct.h"
#include "EnochActorDataFactory.h"
#include "EnochFieldData.h"
#include "EnochFreeLancerData.h"
#include "EnochFieldCellData.h"
#include "EnochProjectileData.h"
#include "../Enoch.h"
#include <memory>

queue<SimulateInfo> EnochSimulator::logs;
float EnochSimulator::simulateTime;
EnochSimulator* EnochSimulator::instance;

EnochSimulator::EnochSimulator()
{
	simulateTime = 0;
	result = GameResult::None;
	simulating = false;
	simulateEndTime = 0;
	numEnemyFreeLancer = 0;
	numPlayerFreeLancer = 0;
	logs = std::queue<SimulateInfo>();
	instance = this;
}

EnochSimulator::~EnochSimulator()
{
	logs = std::queue<SimulateInfo>();
	
}

void EnochSimulator::BeginPlay()
{
	//FreeLancer 큐 초기화. 맵 위의 모든 용병이 큐에 들어감
	//FreeLancerQueue = make_unique<PriorityQueue<FLBattleData>>();
	FreeLancerQueue = make_unique<vector<int>>();
	for(int i=0; i<EnochFieldData::GetHeight(); i++) {
		for (int j=0;j<EnochFieldData::GetWidth();j++) {
			auto cell = EnochFieldData::GetData({i,j});
			auto freeLancer = EnochActorDataFactory::instance->GetFreeLancerData(cell->FreeLancerOn);
			if (freeLancer != nullptr) {
				freeLancer->BeginSimulate();
				cell->FreeLancerOnFight = freeLancer->GetSerialNumber();
				FreeLancerQueue->push_back(freeLancer->GetSerialNumber());
				if (freeLancer->isEnemy) numEnemyFreeLancer++;
				else numPlayerFreeLancer++;
			}
		}
	}
	ENLOG(Warning, TEXT("%d %d"), numEnemyFreeLancer, numPlayerFreeLancer);
	//액터가 없는상태로 시작한 경우의 예외 처리
	//Projectile 큐 초기화
	ProjectileQueue = make_unique<vector<int>>();
	//ProjectileQueue = new PriorityQueue<EnochProjectileData>();
	if (numEnemyFreeLancer == 0 && numPlayerFreeLancer == 0)
		result = GameResult::Draw;
	else if (numEnemyFreeLancer == 0)
		result = GameResult::Win;
	else if (numPlayerFreeLancer == 0)
		result = GameResult::Lose;
	else {
		result = GameResult::UnderSimulated;
		simulating = true;
		simulateEndTime = FLT_MAX;
	}

}

void EnochSimulator::EndPlay()
{
	//Act클래스 정리
	//FreeLancerAct::Release();

	//맵 위의 모든 용병 분리
	for (int i = 0; i < EnochFieldData::GetHeight(); i++)
	{
		for (int j = 0; j < EnochFieldData::GetWidth(); j++)
		{
			auto cell = EnochFieldData::GetData({ i,j });
			if (!cell) continue;
			auto data = EnochActorDataFactory::instance->GetFreeLancerData(cell->FreeLancerOnFight);
			cell->FreeLancerOnFight = -1;
			if (data) {
				data->nextAttackTime = 0;
				data->SetState(FreeLancerState::Idle);
			}
		}
	}

	for (auto it_pj = ProjectileQueue->begin();
		it_pj != ProjectileQueue->end();
		it_pj = ProjectileQueue->erase(it_pj))
		EnochSimulator::logs.push({ SimulateInfoType::RemovePJ, *it_pj });
}

void EnochSimulator::BeginTick(float DeltaTime)
{
	//시뮬레이터의 시간은 Tick이 호출될때만 흐름
}

shared_ptr<pair<vector<int>, vector<int>>> EnochSimulator::Tick(float DeltaTime)
{
	simulateTime += DeltaTime;
	auto results = make_shared<pair<vector<int>, vector<int>>>();

	//시뮬레이션 종료 후 일정시간이 지나 승패판정
	if (!simulating && simulateTime > simulateEndTime) {
		if (numEnemyFreeLancer == 0 && numPlayerFreeLancer == 0)
			result = GameResult::Draw;
		else if (numEnemyFreeLancer == 0)
			result = GameResult::Win;
		else if (numPlayerFreeLancer == 0)
			result = GameResult::Lose;
	}
	numEnemyFreeLancer = 0;
	numPlayerFreeLancer = 0;
	//Tick FreeLancers
	for (auto& SN_FL : *FreeLancerQueue)
		FreeLancerTick(EnochActorDataFactory::instance->GetFreeLancerData(SN_FL), DeltaTime);

	//Tick Projectiles
	for (auto& SN_PJ : *ProjectileQueue)
		ProjectileTick(EnochActorDataFactory::instance->GetProjectileData(SN_PJ), DeltaTime);

	//Remove Expired Projectiles
	for (auto it = ProjectileQueue->begin(); it != ProjectileQueue->end();) {
		auto projectile = EnochActorDataFactory::instance->GetProjectileData(*it);
		if (projectile == nullptr)
			ProjectileQueue->erase(it);
		else if (projectile->expired) {
			EnochSimulator::logs.push({ SimulateInfoType::RemovePJ, projectile->SerialNumber });
			ProjectileQueue->erase(it);
		}
		else it++;
	}

	if (numEnemyFreeLancer == 0 || numPlayerFreeLancer == 0) {
		if (numEnemyFreeLancer == 0 && numPlayerFreeLancer == 0)
			result = GameResult::Draw;
		else if (numEnemyFreeLancer == 0)
			result = GameResult::Win;
		else if (numPlayerFreeLancer == 0)
			result = GameResult::Lose;
		simulating = false;
	}

	//return unused
	return results;
}

void EnochSimulator::FreeLancerTick(FLBattleData *freeLancer, float deltaTime)
{
	//ignore Invalids
	if(freeLancer == nullptr) return;
	
	if(freeLancer->GetState() == FreeLancerState::Dead) return;
	
	if (freeLancer->isEnemy) numEnemyFreeLancer++;
	else numPlayerFreeLancer++;

	//check if Game is Over
	if (!simulating && freeLancer->GetState() != FreeLancerState::Victory) {
		freeLancer->SetState(FreeLancerState::Victory);
		return;
	}
	//modify state
	freeLancer->Act(deltaTime);
}

void EnochSimulator::ProjectileTick(EnochProjectileData* projectile, float deltaTime)
{
	if (projectile == nullptr) {
		EnochSimulator::logs.push({ SimulateInfoType::Debug, 8 });
		return;
	}

	auto target = projectile->_target;

	if (simulating)
		projectile->Act(deltaTime);
}
