// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "EnochActor.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API AEnochActor : public AActor
{
	GENERATED_BODY()
public:
	AEnochActor();
	//모든 동작 시작시 호출하는 함수
	virtual void ActInit();
	class AEnochActorFactory* Factory;

	UFUNCTION(BlueprintCallable, Category = "EnochActor")
	virtual void SetActive(bool active);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
};
