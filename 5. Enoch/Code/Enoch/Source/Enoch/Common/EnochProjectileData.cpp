// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochProjectileData.h"
#include "EnochActorDataFactory.h"
#include "EnochFreeLancerData.h"

EnochProjectileData::EnochProjectileData()
{
	Target = nullptr;
	speed = 0;
}

EnochProjectileData::~EnochProjectileData()
{
	
}

void EnochProjectileData::Set(int damageIn, float speedIn, EnochFreeLancerData* targetIn, EnochFreeLancerData* ownerIn) {
	attackDamage = damageIn;
	speed = speedIn;
	Target = targetIn;
	Owner = ownerIn;
	pos = VectorF2D(Owner->locationOnFight);
}

void EnochProjectileData::Release()
{
	if(EnochActorDataFactory::instance)
		EnochActorDataFactory::instance->Release(this);
}