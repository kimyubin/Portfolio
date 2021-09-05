// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnochFieldData.h"
/**
 * 필드 중 한칸에 대한 데이터
 */
class ENOCH_API EnochFieldCellData
{
public:
	EnochFieldCellData(int inputY, int inputX) : location({inputY,inputX}) {};
	~EnochFieldCellData();
	void SetFreeLancerOn(class EnochFreeLancerData *arg);
	//언리얼엔진 코드에서 실행하지 말 것
	void SetFreeLancerOnFight(class EnochFreeLancerData* arg);
	Vector2D GetLocation() { return location; }
private :
private:
	Vector2D location;
	class EnochFreeLancerData *FreeLancerOn;		//현재 이 셀을 밟고있는 프리랜서(비전투중)
	class EnochFreeLancerData* FreeLancerOnFight;	//현재 이 셀을 밟고있는 프리랜서(전투중)
	friend class EnochSimulator;
	friend class EnochFiniteStateMachine;
	friend class EnochFreeLancerAct;
};

