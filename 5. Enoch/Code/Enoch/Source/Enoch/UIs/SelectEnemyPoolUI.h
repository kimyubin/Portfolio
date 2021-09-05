// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "EnemyPoolPanel.h"
#include "Blueprint/UserWidget.h"
#include "SelectEnemyPoolUI.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API USelectEnemyPoolUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	UPROPERTY(meta = (BindWidget))
	UEnemyPoolPanel* EnemyPoolPanel_0;
	UPROPERTY(meta = (BindWidget))
	UEnemyPoolPanel* EnemyPoolPanel_1;
	UPROPERTY(meta = (BindWidget))
	UEnemyPoolPanel* EnemyPoolPanel_2;
	
};
