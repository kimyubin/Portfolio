// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EMGamePlayStatics.generated.h"

enum class UnitSkin : uint8;
/**
 * 공용 유틸리티 함수를 가지고 있는 Static Class
 */
UCLASS()
class EVERYMINO_API UEMGamePlayStatics : public UObject
{
	GENERATED_BODY()
public:
	
	/**
	 * 유닛 스폰 처리. 스폰하고 부모 액터에 붙여줌.
	 * @return 스킨이 비어있으면 nullptr 리턴, 그렇지 않으면 스폰후 유닛 리턴.
	 */
	static class AEMUnitBrick* SpawnUnit(int InX, int InY, UnitSkin SkinEnum, AActor* ParentActor);

	/**
	 * 유닛 스폰 처리. 스폰하고 부모 액터에 붙여줌.
	 * @return 스킨이 비어있으면 nullptr 리턴, 그렇지 않으면 스폰후 유닛 리턴.
	 */
	static class AEMUnitBrick* SpawnUnit(FVector2D InLocation, UnitSkin SkinEnum, AActor* ParentActor);

	/** 유닛 파괴 처리. 파괴하고 nullptr처리까지함.*/
	static void DestroyUnit(AEMUnitBrick** InUnit);
};
