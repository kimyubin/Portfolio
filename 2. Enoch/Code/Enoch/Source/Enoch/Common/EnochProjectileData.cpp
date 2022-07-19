// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochProjectileData.h"
#include "EnochActorDataFactory.h"
#include "EnochFreeLancerData.h"
#include "PJActs/PJGuidedArrow.h"


EnochProjectileData::~EnochProjectileData()
{
	
}

void EnochProjectileData::Set(int damageIn, float speedIn, int target, int owner) {
	attackDamage = damageIn;
	speed = speedIn;
	_target = target;
	_owner = owner;
	auto ownerData = EnochActorDataFactory::instance->GetFreeLancerData(_owner);
	
	location = VectorF2D(ownerData->locationOnFight);

	//탄환에도 타입이 필요함. 유도, 비유도, 관통, 범위 등
	act = make_shared<PJGuidedArrow>();
	act->data = this;
	act->Begin();
}

void EnochProjectileData::Act(float deltaTime)
{
	act->Tick(deltaTime);
}

void EnochProjectileData::UpdateTime(float deltaTime)
{
	//추후 시간에 따른 데이터 변화 적용을 위해 만들어만 둔 함수
}