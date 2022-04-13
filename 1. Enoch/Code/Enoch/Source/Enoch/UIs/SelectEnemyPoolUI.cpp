// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "SelectEnemyPoolUI.h"
#include "EnemyPoolDetailsPanel.h"
#include "EnemyPoolPanel.h"

USelectEnemyPoolUI* USelectEnemyPoolUI::SelectEnemyPoolUIPtr;

void USelectEnemyPoolUI::NativeConstruct()
{
	Super::NativeConstruct();
	SelectEnemyPoolUIPtr = this;
	EnemyPoolDetailsPanel->SetSelectEnemyPoolUI(this);

	//적풀 패널에 자신의 번호를 보내고 그걸 바탕으로 각자 적풀에 있는 용병 숫자정리.
	//EnemyPoolFLbyGradeList로 정리된 데이터를 받아옴
	EnemyPoolPanel_0->SetEnemyPoolData(0);
	EnemyPoolPanel_1->SetEnemyPoolData(1);
	EnemyPoolPanel_2->SetEnemyPoolData(2);
}

UMaterialInterface* USelectEnemyPoolUI::GetSlotImg(const int FreeLancerNum)
{
	auto modelID = FreeLancerTemplate::GetFreeLancerModelID(FreeLancerNum);
	return SelectEnemySlotDataArr[modelID].Material;
}

UMaterialInterface* USelectEnemyPoolUI::GetSlotImg(const uint8 FreeLancerTID)
{
	auto modelID = FreeLancerTemplate::GetFreeLancerModelID(FreeLancerTID);
	return SelectEnemySlotDataArr[modelID].Material;
}

void USelectEnemyPoolUI::UpdateDetailPanel(const int& PoolNum, int GradeNum)
{
	EnemyPoolDetailsPanel->UpdateDetailData(PoolNum, GradeNum);
}