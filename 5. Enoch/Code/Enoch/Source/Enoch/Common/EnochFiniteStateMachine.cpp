// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFiniteStateMachine.h"
#include "EnochFieldData.h"
#include "EnochFieldCellData.h"
#include "EnochFreeLancerState.h"
#include "EnochFreeLancerData.h"
#include <algorithm>
#include <queue>
#include <vector>

void EnochFiniteStateMachine::Decide(EnochFreeLancerData *freeLancer)
{
	//현재는 스킬 구현이 없으므로 무의미. 스킬이 있는 경우 스킬 우선
	//if (freeLancer->CanUseSpell())
	//	;

	Vector2D attackTarget;
	if(freeLancer->target != nullptr)
		attackTarget = freeLancer->target->locationOnFight;
	else 
	{
		//새 타겟 탐색
		attackTarget = GetClosest(freeLancer, 0,
			[target = !freeLancer->isEnemy](EnochFreeLancerData* freeLancer2){
			return freeLancer2->isEnemy == target;
		});
		//공격대상이 맵에 없는 경우 Idle로 결정하고 종료
		if (attackTarget.x == -1) {
			freeLancer->SetState(FreeLancerState::Idle);
			return;
		}
		freeLancer->target = EnochFieldData::GetData(attackTarget)->FreeLancerOnFight;
	}


	//공격대상이 있고 공격이 가능한 거리인경우 공격으로 결정하고 종료.
	if (freeLancer->attackRange >= CalculateDistance(freeLancer->locationOnFight, attackTarget)) {
		freeLancer->SetState(FreeLancerState::PreAttack);
		return;
	}

	//공격대상이 있지만 공격이 불가능한 거리인 경우, 대상까지의 경로 중 첫 이동경로를 찾음
	auto FirstMove = FindPath(freeLancer, attackTarget);

	//공격대상까지 이동이 불가능한 경우, Idle로 결정하고 종료
	if (FirstMove == freeLancer->locationOnFight) {
		freeLancer->SetState(FreeLancerState::Idle);
		return;
	}

	//공격대상까지 이동이 가능한 경우, 이동하기로 결정하고 종료
	freeLancer->SetState(FreeLancerState::PreMove);
	freeLancer->moveSrcLocation = freeLancer->locationOnFight;
	freeLancer->moveDstLocation = FirstMove;
}

//constraint에 맞는 가장 가까운 프리랜서 찾기
Vector2D EnochFiniteStateMachine::GetClosest(EnochFreeLancerData* freeLancer, int owner, std::function<bool(EnochFreeLancerData*)>constraint)
{
	auto xNow = freeLancer->locationOnFight.x;
	auto yNow = freeLancer->locationOnFight.y;
	EnochFieldCellData* cell;

	//////////////////////////////////////////
#define CHECKCELL(y, x) \
	cell = EnochFieldData::GetData({y , x}); \
	if (cell && cell->FreeLancerOnFight && constraint(cell->FreeLancerOnFight)) \
		return cell->FreeLancerOnFight->locationOnFight;
	//////////////////////////////////////////


	//자신주변 사각형을 순차적으로 탐색하되, 사각형의 크기를 점점 키워나감
	//거리가 같을 경우, 대각선보다 직선경로에 가까운 타깃을 우선함
	for (int distance = 1; distance < 10; distance++)
	{
		//직선경로 우선하여 계산
		CHECKCELL(yNow + distance , xNow);
		CHECKCELL(yNow - distance , xNow);
		CHECKCELL(yNow , xNow + distance);
		CHECKCELL(yNow , xNow - distance);

		//slope는 직선에서 벗어난 정도를 의미. slope == distance인 경우 45도 대각선
		for (int slope = 1; slope < distance; slope++)
		{
			//위
			CHECKCELL(yNow + distance, xNow + slope);
			CHECKCELL(yNow + distance, xNow - slope);
			//아래
			CHECKCELL(yNow - distance, xNow + slope);
			CHECKCELL(yNow - distance, xNow - slope);
			//왼쪽
			CHECKCELL(yNow + slope, xNow - distance);
			CHECKCELL(yNow - slope, xNow - distance);
			//오른쪽
			CHECKCELL(yNow + slope, xNow + distance);
			CHECKCELL(yNow - slope, xNow + distance);
		}

		//가장 먼 45도 대각선 경로
		CHECKCELL(yNow + distance, xNow + distance);
		CHECKCELL(yNow - distance, xNow + distance);
		CHECKCELL(yNow + distance, xNow - distance);
		CHECKCELL(yNow - distance, xNow - distance);
	}
	return { -1,-1 };
}

inline int EnochFiniteStateMachine::CalculateDistance(Vector2D src, Vector2D dst)
{
	return std::max(
		std::abs(src.x - dst.x),
		std::abs(src.y - dst.y)
		);
}

Vector2D EnochFiniteStateMachine::FindPath(EnochFreeLancerData* freeLancer, Vector2D dst)
{
	auto src = freeLancer->locationOnFight;
	auto q = std::queue<Vector2D>();
	
	auto map = std::vector<std::vector<bool>>();
	//맵을 true로 초기화된 10x10 크기로 생성
	map.resize(10,std::vector<bool>(10,true));
	map[dst.y][dst.x] = false;
	q.push({dst.y, dst.x});

	////////////////////////////////////////////////////
#define CHECKPATH(yy, xx) \
	data = EnochFieldData::GetData({yy , xx}); \
	if (data && map[ yy ][ xx ] && !data->FreeLancerOnFight) { \
		map[ yy ][ xx ] = false; \
		q.push({yy , xx}); \
	}
	////////////////////////////////////////////////////

	//최단경로를 찾고 첫노드를 찾기는 어려우므로, 역방향 최단경로를 찾는 방법 사용
	//인게임에선 대각선이동이 가능하지만, 자연스럽게 이동하는걸로 보이려면 직선이동 기준으로 루트를 짜야함
	while (!q.empty())
	{
		auto node = q.front();
		EnochFieldCellData *data;
		q.pop();

		if (CalculateDistance(src, node) <= 1) return node;
		else 

		CHECKPATH(node.y + 1 , node.x);
		CHECKPATH(node.y , node.x + 1);
		CHECKPATH(node.y , node.x - 1);
		CHECKPATH(node.y - 1 , node.x);
	}

	//전부 불가능한 경우..
	return {src.y,src.x};
}
