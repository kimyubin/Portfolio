// Fill out your copyright notice in the Description page of Project Settings.

#include "EnochSimulator.h"
#include "EnochFiniteStateMachine.h"
#include "EnochFreeLancerAct.h"
#include "EnochActorDataFactory.h"
#include "EnochFieldData.h"
#include "EnochFreeLancerData.h"
#include "EnochFieldCellData.h"
#include "EnochProjectileData.h"

#include <memory>

queue<SimulateInfo> EnochSimulator::logs;

EnochSimulator::EnochSimulator()
{
	simulateTime = 0;
	result = GameResult::None;
	simulating = false;
	simulateEndTime = 0;
	numEnemyFreeLancer = 0;
	numPlayerFreeLancer = 0;
	logs = std::queue<SimulateInfo>();
}

EnochSimulator::~EnochSimulator()
{
	logs = std::queue<SimulateInfo>();
	
}

void EnochSimulator::BeginPlay()
{
	//Act클래스 초기화
	EnochFreeLancerAct::Init(this);

	//FreeLancer 큐 초기화. 맵 위의 모든 용병이 큐에 들어감
	//FreeLancerQueue = make_unique<PriorityQueue<EnochFreeLancerData>>();
	FreeLancerQueue = new PriorityQueue<EnochFreeLancerData>();
	for(int i=0; i<EnochFieldData::GetHeight(); i++) {
		for (int j=0;j<EnochFieldData::GetWidth();j++) {
			auto cell = EnochFieldData::GetData({i,j});
			if (cell->FreeLancerOn != nullptr) {
				cell->FreeLancerOn->BeginSimulate();
				cell->FreeLancerOnFight = cell->FreeLancerOn;
				FreeLancerQueue->push({ cell->FreeLancerOn, 0.0f });
				if (cell->FreeLancerOn->isEnemy) numEnemyFreeLancer++;
				else numPlayerFreeLancer++;
			}
		}
	}

	//Projectile 큐 초기화
	//ProjectileQueue = make_unique<PriorityQueue<EnochProjectileData>>();
	ProjectileQueue = new PriorityQueue<EnochProjectileData>();
	//액터가 없는상태로 시작한 경우의 예외 처리
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
	EnochFreeLancerAct::Release();

	//맵 위의 모든 용병 분리
	for (int i = 0; i < EnochFieldData::GetHeight(); i++)
	{
		for (int j = 0; j < EnochFieldData::GetWidth(); j++)
		{
			auto cell = EnochFieldData::GetData({ i,j });
			if (!cell) continue;
			cell->FreeLancerOnFight = nullptr;
		}
	}

	//큐 비우기
	while (FreeLancerQueue && !FreeLancerQueue->empty())
	{
		auto left = FreeLancerQueue->top();
		FreeLancerQueue->pop();
	}
	delete FreeLancerQueue;
	delete ProjectileQueue;
	FreeLancerQueue = nullptr;
	ProjectileQueue = nullptr;
}

void EnochSimulator::BeginTick(float DeltaTime)
{
	//시뮬레이터의 시간은 Tick이 호출될때만 흐름
}

shared_ptr<pair<vector<int>,vector<int>>> EnochSimulator::Tick(float DeltaTime)
{
	simulateTime += DeltaTime;
	auto results = make_shared<pair<vector<int>,vector<int>>>();
	auto& result_FL = std::get<0>(*results);
	auto& result_PJ = std::get<1>(*results);

	//시뮬레이션 종료 후 일정시간이 지나 승패판정
	if (!simulating && simulateTime > simulateEndTime) {
		if (numEnemyFreeLancer == 0 && numPlayerFreeLancer == 0)
			result = GameResult::Draw;
		else if (numEnemyFreeLancer == 0)
			result = GameResult::Win;
		else if (numPlayerFreeLancer == 0)
			result = GameResult::Lose;
	}

	Node<EnochFreeLancerData> freelancer;
	Node<EnochProjectileData> projectile;
	while (!FreeLancerQueue->empty() && (freelancer = FreeLancerQueue->top()).time <= simulateTime) {
		FreeLancerQueue->pop();
		result_FL.push_back(FreeLancerTick(freelancer));
	}
	while (!ProjectileQueue->empty() && (projectile = ProjectileQueue->top()).time <= simulateTime) {
		ProjectileQueue->pop();
		result_PJ.push_back(ProjectileTick(projectile));
	}
	
	return results;
}

int EnochSimulator::FreeLancerTick(Node<EnochFreeLancerData>& node)
{
	auto freeLancer = node.data;
	
	//시뮬레이션이 종료된 경우 
	if (!simulating) {
		//남은 액터들은 모두 승리포즈를 취하며, 더이상 동작하지 않으므로 push 없음
		freeLancer->SetState(FreeLancerState::Victory);
		return freeLancer->SerialNumber;
	}

	//행동 후, 필요시 다음 동작 결정
	if(EnochFreeLancerAct::Act(freeLancer))
		EnochFiniteStateMachine::Decide(freeLancer);

	//프리랜서가 죽은 경우 큐의 맨뒤(전투 시간제한 이후)로 이동
	if (freeLancer->GetState() != FreeLancerState::Dead){
		FreeLancerQueue->push({ freeLancer, node.time + freeLancer->stateDuration });
	}
	else {
		//프리랜서 유닛 숫자 차감 및 한쪽이라도 0명이 된 경우 시뮬레이션 종료
		if ((freeLancer->isEnemy && --numEnemyFreeLancer == 0) ||
			(!freeLancer->isEnemy && --numPlayerFreeLancer == 0))
		{
			simulating = false;
			simulateEndTime = simulateTime + postDelay;
		}
	}
	//어떤 프리랜서가 데이터 변동을 일으켰는지 리턴하여 엔진에 알림. 엔진은 이를 기반으로 렌더링
	return freeLancer->SerialNumber;
}

int EnochSimulator::ProjectileTick(Node<EnochProjectileData>& node)
{
	auto projectile = node.data;
	auto target = projectile->Target;

	//if (simulating) 
	//	EnochFreeLancerData::DoAttack(projectile, target);
	
	return projectile->SerialNumber;
}
