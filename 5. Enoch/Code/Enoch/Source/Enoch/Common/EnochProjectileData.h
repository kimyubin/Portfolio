// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>
#include "EnochGeometry.h"

using namespace std;

enum class ProjectileTemplateID : uint8 {
	None = 0, //NULL
    RedDot = 1,
	TypeNum,
};
/**
 * 
 */
class ENOCH_API EnochProjectileData
{
public:
	EnochProjectileData();
	~EnochProjectileData();
	void Release();
	void Set(int damageIn, float speedIn, class EnochFreeLancerData* targetIn, class EnochFreeLancerData* ownerIn);
	int attackDamage;

	int SerialNumber;
public:
	float speed; //투사체 속도
	VectorF2D pos;
	class EnochFreeLancerData* Target;	//투사체 타겟
	class EnochFreeLancerData* Owner;	//투사체 발사자
	
	//각종 버프, 디버프는 나중에 추가.
};
