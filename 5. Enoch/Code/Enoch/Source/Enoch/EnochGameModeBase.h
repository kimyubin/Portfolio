// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Enoch.h"
#include "Common/EnochFreeLancerData.h"
#include "Blueprint/UserWidget.h"
#include "Common/EnochSimulator.h"
#include "GameFramework/GameModeBase.h"
#include "EnochGameModeBase.generated.h"


UCLASS()
class ENOCH_API AEnochGameModeBase : public AGameModeBase
{	
	GENERATED_BODY()		

public:
	AEnochGameModeBase();

	//엔진에서 사용할 위젯 클래스를 가져옴
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> StartWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> ResultWidgetClass;

	
	UFUNCTION(BlueprintCallable, Category = "UMG Game")	
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);	
		
	/**결과 화면용
	 *Enochfight에서 받아옴*/	
	GameResult FightResult;
	void SetFightResult(const GameResult& InResult);
	GameResult GetFightResult(){return FightResult;}
	//결과 위젯 화면에 붙임
	void CallResultUI();

protected:
	virtual void BeginPlay() override;

	/**
	 * 레벨 시작시 사용될 위젯UI
	 * 월드 별 초기 UI를 지정함
	 */	
	UPROPERTY()
	UUserWidget* CurrentWidget;
	/** 결과 화면 위젯 생성용*/
	UPROPERTY()
	UUserWidget* ResultUI;
};