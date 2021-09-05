// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "EnochGameModeBase.h"
#include "EnochPaperFlipbookComponent.h"
#include "EnochActorFactory.h"
#include "EnochFreeLancer.h"
#include "Camera/CameraActor.h"
#include "EnochMouseController.h"


AEnochGameModeBase::AEnochGameModeBase()
{
	DefaultPawnClass = ACameraActor::StaticClass(); // 자꾸 디폴트 폰이 나와서 맵에 그림자 만들길래 넣은 코드(카메라는 그림자가 없으니까)
	PlayerControllerClass = AEnochMouseController::StaticClass();	//마우스컨트롤러
	FightResult = GameResult::UnderSimulated;
}

void AEnochGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	ChangeMenuWidget(StartWidgetClass);	
}
void AEnochGameModeBase::SetFightResult(const GameResult& InResult)
{
	//전투결과를 틱마다 업데이트 받고, 결과가 나오면 뷰포트에 결과 UI 붙임.
	FightResult=InResult;
	if(FightResult != GameResult::UnderSimulated) // && FightResult!=GameResult::None)
	{
		CallResultUI();
		FightResult = GameResult::UnderSimulated;
	}
}

void AEnochGameModeBase::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}
void AEnochGameModeBase::CallResultUI()
{
	ResultUI = CreateWidget<UUserWidget>(GetWorld(),ResultWidgetClass);
	if (ResultUI != nullptr)
		ResultUI->AddToViewport(3);
}