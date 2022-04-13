// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFiniteStateMachine.h"
#include "EnochFieldData.h"
#include "EnochActorDataFactory.h"
#include "EnochFieldCellData.h"
#include "EnochFreeLancerState.h"
#include "EnochFreeLancerData.h"

#include "FLActs/FLMove.h"

#include <algorithm>
#include <queue>
#include <vector>

#include "../Enoch.h"
void EnochFiniteStateMachine::Decide(FLBattleData *freeLancer)
{
	
	//if (freeLancer->CanUseSpell())
	//	;
	Vector2D attackTarget;
	{
		attackTarget = GetClosest(freeLancer, 0,
			[target = !freeLancer->isEnemy](FLBattleData* freeLancer2){
			return freeLancer2->isEnemy == target && freeLancer2->GetState() != FreeLancerState::Dead;
		});
		if (attackTarget.x == -1) {
			freeLancer->SetState(FreeLancerState::Idle);
			return;
		}
		freeLancer->SetTarget(EnochFieldData::GetData(attackTarget)->FreeLancerOnFight);
	}


	//공격대상이 있고 공격이 가능한 거리인경우 공격으로 결정하고 종료.
	if (freeLancer->attackRange >= CalculateDistance(freeLancer->locationOnFight, attackTarget)) {
		if (freeLancer->CanAttack())
			freeLancer->SetState(FreeLancerState::PreAttack);
		else
			freeLancer->SetState(FreeLancerState::Idle);
		return;
	}
	

	//공격대상이 있지만 공격이 불가능한 거리인 경우, 대상까지의 경로 중 첫 이동경로를 찾음
	auto FirstMove = freeLancer->canJump ? FindJumpPath(freeLancer) : FindPath(freeLancer, attackTarget);

	//공격대상까지 이동이 불가능한 경우, Idle로 결정하고 종료
	if (FirstMove == freeLancer->locationOnFight) {
		freeLancer->SetState(FreeLancerState::Idle);
		return;
	}

	//공격대상까지 이동이 가능한 경우, 이동하기로 결정하고 종료
	auto rawActInst = freeLancer->actMap->at((int)FreeLancerState::PreMove);
	if (rawActInst == nullptr) {
		freeLancer->SetState(FreeLancerState::Idle);
		return;
	}
	auto targetCellData = EnochFieldData::GetData(FirstMove);
	if (targetCellData == nullptr) {
		freeLancer->SetState(FreeLancerState::Idle);
		return;
	}
	auto moveInst = static_pointer_cast<FLMove>(rawActInst);
	moveInst->prevLocation = freeLancer->locationOnFight;
	moveInst->postLocation = FirstMove;
	targetCellData->FreeLancerOnFight = freeLancer->GetSerialNumber();
	freeLancer->SetState(FreeLancerState::PreMove);
}

//get closest target
Vector2D EnochFiniteStateMachine::GetClosest(FLBattleData* freeLancer, int owner, std::function<bool(FLBattleData*)>constraint)
{
	auto xNow = freeLancer->locationOnFight.x;
	auto yNow = freeLancer->locationOnFight.y;
	EnochFieldCellData* cell;

	//////////////////////////////////////////
#define CHECKCELL(y, x) \
	cell = EnochFieldData::GetData({y , x}); \
	if (cell) { \
		freeLancer2 = EnochActorDataFactory::instance->GetFreeLancerData(cell->FreeLancerOnFight); \
		if (freeLancer2 && constraint(freeLancer2)) \
			return freeLancer2->locationOnFight; \
	}
	//////////////////////////////////////////

	FLBattleData* freeLancer2 = nullptr;
	for (int distance = 1; distance < 10; distance++)
	{
		//90n degree check
		CHECKCELL(yNow + distance , xNow);
		CHECKCELL(yNow - distance , xNow);
		CHECKCELL(yNow , xNow + distance);
		CHECKCELL(yNow , xNow - distance);

		//90n + (-44 ~ 44) degree check
		for (int slope = 1; slope < distance; slope++)
		{
			CHECKCELL(yNow + distance, xNow + slope);
			CHECKCELL(yNow + distance, xNow - slope);
			
			CHECKCELL(yNow - distance, xNow + slope);
			CHECKCELL(yNow - distance, xNow - slope);
			
			CHECKCELL(yNow + slope, xNow - distance);
			CHECKCELL(yNow - slope, xNow - distance);
			
			CHECKCELL(yNow + slope, xNow + distance);
			CHECKCELL(yNow - slope, xNow + distance);
		}

		//90n + 45 degree check
		CHECKCELL(yNow + distance, xNow + distance);
		CHECKCELL(yNow - distance, xNow + distance);
		CHECKCELL(yNow + distance, xNow - distance);
		CHECKCELL(yNow - distance, xNow - distance);
	}
	//no result
	return { -1,-1 };
}

inline int EnochFiniteStateMachine::CalculateDistance(Vector2D src, Vector2D dst)
{
	return std::max(
		std::abs(src.x - dst.x),
		std::abs(src.y - dst.y)
		);
}

Vector2D EnochFiniteStateMachine::FindPath(FLBattleData* freeLancer, Vector2D dst)
{
	auto src = freeLancer->locationOnFight;
	auto q = std::queue<Vector2D>();
	
	auto checkMap = std::vector<std::vector<bool>>();
	//맵을 true로 초기화된 10x10 크기로 생성
	checkMap.resize(10,std::vector<bool>(10,true));
	checkMap[dst.y][dst.x] = false;
	q.push({dst.y, dst.x});

	////////////////////////////////////////////////////
	auto _CheckPath = [&checkMap = checkMap, &q = q](int y, int x) {
		auto data = EnochFieldData::GetData({ y , x });
		if (data && checkMap.at(y).at(x) && data->FreeLancerOnFight == -1) {
			checkMap[y][x] = false;
			q.push({ y , x });
		}
	};
#define CHECKPATH(yy, xx) \
	data = EnochFieldData::GetData({yy , xx}); \
	if (data && map[ yy ][ xx ] && data->FreeLancerOnFight == -1 ) { \
		map[ yy ][ xx ] = false; \
		q.push({yy , xx}); \
	}
	////////////////////////////////////////////////////

	//최단경로를 찾고 첫노드를 찾기는 어려우므로, 역방향 최단경로를 찾는 방법 사용
	//인게임에선 대각선이동이 가능하지만, 자연스럽게 이동하는걸로 보이려면 직선이동 기준으로 루트를 짜야함
	while (!q.empty())
	{
		auto node = q.front();
		q.pop();
		if (CalculateDistance(src, node) <= 1) return node;
		_CheckPath(node.y + 1, node.x);
		_CheckPath(node.y, node.x + 1);
		_CheckPath(node.y, node.x - 1);
		_CheckPath(node.y - 1, node.x);
	}

	//전부 불가능한 경우..
	return {src.y,src.x};
}

Vector2D EnochFiniteStateMachine::FindJumpPath(FLBattleData* freeLancer)
{
	auto src = freeLancer->locationOnFight;
	auto q = std::queue<Vector2D>();

	auto dst = freeLancer->location;
	for(auto i=0; i<EnochFieldData::GetHeight(); i++)
		for (auto j = 0; j < EnochFieldData::GetWidth(); j++)
		{
			auto data = EnochFieldData::GetData({ i , j });
			if (data == nullptr) continue;
			//if (VectorF2D(freeLancer->location).distance(dst) < VectorF2D(freeLancer->location).distance(data->location))
		}

	auto checkMap = std::vector<std::vector<bool>>();
	//맵을 true로 초기화된 10x10 크기로 생성
	checkMap.resize(10, std::vector<bool>(10, true));
	checkMap[dst.y][dst.x] = false;
	q.push({ dst.y, dst.x });

	////////////////////////////////////////////////////
	auto _CheckPath = [&checkMap = checkMap, &q = q](int y, int x) {
		auto data = EnochFieldData::GetData({ y , x });
		if (data && checkMap.at(y).at(x) && data->FreeLancerOnFight == -1) {
			checkMap[y][x] = false;
			q.push({ y , x });
		}
	};
#define CHECKPATH(yy, xx) \
	data = EnochFieldData::GetData({yy , xx}); \
	if (data && map[ yy ][ xx ] && data->FreeLancerOnFight == -1 ) { \
		map[ yy ][ xx ] = false; \
		q.push({yy , xx}); \
	}
	////////////////////////////////////////////////////

	//최단경로를 찾고 첫노드를 찾기는 어려우므로, 역방향 최단경로를 찾는 방법 사용
	//인게임에선 대각선이동이 가능하지만, 자연스럽게 이동하는걸로 보이려면 직선이동 기준으로 루트를 짜야함
	while (!q.empty())
	{
		auto node = q.front();
		q.pop();
		if (CalculateDistance(src, node) <= 1) return node;
		_CheckPath(node.y + 1, node.x);
		_CheckPath(node.y, node.x + 1);
		_CheckPath(node.y, node.x - 1);
		_CheckPath(node.y - 1, node.x);
	}

	//전부 불가능한 경우..
	return { src.y,src.x };
}
