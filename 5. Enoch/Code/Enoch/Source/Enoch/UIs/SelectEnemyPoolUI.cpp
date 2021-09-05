// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectEnemyPoolUI.h"

void USelectEnemyPoolUI::NativeConstruct()
{
	Super::NativeConstruct();
	EnemyPoolPanel_0->SetEnemyPoolData(0);
	EnemyPoolPanel_1->SetEnemyPoolData(1);
	EnemyPoolPanel_2->SetEnemyPoolData(2);
}
