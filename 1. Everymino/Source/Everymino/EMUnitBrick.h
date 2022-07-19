// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BrickTemplate.h"

#include "GameFramework/Actor.h"
#include "EMUnitBrick.generated.h"

UCLASS()
class EVERYMINO_API AEMUnitBrick : public AActor
{
	GENERATED_BODY()
	
public:	
	AEMUnitBrick();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	void SetSkinMesh(uint8 SkinNum = 0);
	void SetSkinMesh(UnitSkin SkinEnum);
	void SetPositionAndSkin(int InX, int InY, UnitSkin SkinEnum);

	/** 유닛의 상대좌표를 실제 좌표로 변환해서 셋팅*/
	void SetPosition(int InX, int InY);
	
	UnitSkin MySkinNum;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* CubeStaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D MyPosition;
};
