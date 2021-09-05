﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <queue>
#include <vector>
#include <memory>
#include <utility>
#include <unordered_map>
#include "EnochFreeLancerData.h"
#include "EnochProjectileData.h"

using namespace std;

/*
* 시간순으로 노드를 돌려주는 우선순위 큐에 모든 사건을 넣으며 시뮬레이터 진행
* 각 노드는 3개의 값을 가짐 (데이터 타입, 데이터, 시간)
 */

UENUM()
enum class GameResult : uint8 {
	None,
	UnderSimulated,
	Win,
	Lose,
	Draw,
};


UENUM()
enum class SimulateInfoType : uint8 {
	MeleeAttack,
};

struct SimulateInfo {
	SimulateInfoType type;
	int giver;
	int taker;
	float value;
};

class ENOCH_API EnochSimulator
{
public:
	EnochSimulator();
	~EnochSimulator();
	void BeginPlay();
	void EndPlay();
	void BeginTick(float DeltaTime);
	shared_ptr<pair<vector<int>, vector<int>>> Tick(float DeltaTime);

	//시뮬레이터 결과
	GameResult result;
	static queue<SimulateInfo> logs;
	
	
private:
	//큐에 사용될 노드
	template<typename T>
	struct Node {
		T* data;
		float time;
	};

	//노드의 정렬에 사용될 비교함수. 가장 최근의 일을 먼저 리턴하도록 정렬
	template<typename T>
	struct comp {
		bool operator()(struct Node<T> &lhs, struct Node<T> &rhs) {
			return lhs.time > rhs.time;
		}
	};

	template<typename T>
	using PriorityQueue = priority_queue < struct Node<T>, vector<Node<T>>, comp<T> >;

	//template<typename T>
	//using QueuePtr = unique_ptr <PriorityQueue<T>>;

	
	float simulateTime;	//시뮬레이터 내 시간
	float simulateEndTime;	//시뮬레이터가 종료될 시간
	const float postDelay = 3.0f;	//시뮬레이터가 종료된 후 판정이 일어날 때 까지의 시간
	int numPlayerFreeLancer;	//아군수
	int numEnemyFreeLancer;		//적군수
	bool simulating;	//시뮬레이터가 작동중인 경우 true
	


	int FreeLancerTick(Node<EnochFreeLancerData> &node);
	int ProjectileTick(Node<EnochProjectileData> &node);

public:
	//용병 우선순위 큐
	PriorityQueue<EnochFreeLancerData> *FreeLancerQueue = nullptr;

	//투사체 우선순위 큐
	PriorityQueue<EnochProjectileData> *ProjectileQueue = nullptr;

};