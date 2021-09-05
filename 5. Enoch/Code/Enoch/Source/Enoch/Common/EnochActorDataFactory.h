// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include<vector>
#include<queue>
#include<memory>

using namespace std;

/**
 *  액터 데이터 관리자
 *  미리생성하는 로직은 밀어버리고, 제한된 인덱스를 재활용하는 방식 사용
 */
class ENOCH_API EnochActorDataFactory
{
public:
	EnochActorDataFactory();
	~EnochActorDataFactory();

	void BeginPlay();
	void EndPlay();

	//프리랜서 데이터를 하나 스폰하고, 인덱스(시리얼넘버) 리턴
	int SpawnFreeLancer();

	//투사체 데이터를 하나 스폰하고, 인덱스(시리얼넘버) 리턴
	int SpawnProjectile();
	
	void Release(class EnochProjectileData *projectile); //투사체 인덱스를 반납하고 데이터 제거
	void Release(class EnochFreeLancerData* freelancer); //용병 인덱스를 반납하고 데이터 제거
	
	inline class EnochFreeLancerData* GetFreeLancerData(int SN) {
		return _freelancerMap->at(SN);
	}

	inline class EnochProjectileData* GetProjectileData(int SN) {
		return _projectileMap->at(SN);
	}
	static EnochActorDataFactory *instance;

private :
	//용병정보가 할당된 맵. 크기가 0인상태로 시작하며, 부족할때만 크기를 늘림
	unique_ptr<vector<class EnochFreeLancerData*>> _freelancerMap;
	//용병정보 맵의 빈자리를 기억하고 하나씩 알려주는 큐
	unique_ptr<queue<int>> _freelancerQueue;

	//투사체가 할당된 맵
	unique_ptr<vector<class EnochProjectileData*>> _projectileMap;
	//투사체 맵의 빈자리를 기억하고 하나씩 알려주는 큐
	unique_ptr<queue<int>> _projectileQueue;
};
