// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochActor.h"

AEnochActor::AEnochActor()
{
	;
}

void AEnochActor::BeginPlay()
{
	Super::BeginPlay();
}
void AEnochActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AEnochActor::SetActive(bool active)
{
	SetActorHiddenInGame(!active);	//화면 렌더 여부
	if (!active) SetActorTransform(FTransform(FVector(-1000.0f, 0.0f, 0.0f))); //게임에 영향을 못끼치게 멀리 떨어뜨림
	SetActorTickEnabled(active);	//틱 호출 통제
}

void AEnochActor::ActInit()
{
	;
}