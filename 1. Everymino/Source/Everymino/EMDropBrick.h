// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BrickTemplate.h"
#include "GameFramework/Actor.h"
#include "EMDropBrick.generated.h"

UCLASS()
class EVERYMINO_API AEMDropBrick : public AActor
{
	GENERATED_BODY()
	
public:	
	AEMDropBrick();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	void InitBrick(class ABrickBoardManager* InOwnerBoardManager, EDropStartDirection InStartDirection);
	void NewBrick();
	void SpinBrick(BrickDirection Dir);
	void SpinBrick(EMInput SpinInput);
	
	/**
	 * DroppingBrick정보를 바탕으로 DroppingUnits을 업데이트함.
	 * 변경점이 있는 경우만 업데이트 그외에는 기존 유닛 사용
	 */
	void UpdateDroppingBrick();
	
	/** DropBrickData 회전 */
	void SpinDroppingBrick(EMInput InInput);
	
	/** DropBrickData 이동 좌,우, 소프트, 하드 드롭 지정	 */
	void MoveDroppingBrick(EMInput InInput);
	void MoveBasePoint(EMInput InInput);
	void MoveSoftDrop();
	void MoveHardDrop();
	void MoveBrickUnits();
	void InputHandler(EMInput InInput);
	bool CheckBoard();

	/** 인덱스 값에 대한 스킨 정보*/
	UnitSkin GetUnitSkin(int InX, int InY) const { return Brick[InY][InX]; }

	/** 원점 대비 유닛의 상대좌표.*/
	FVector2D GetUnitLocation(int InX, int InY) const;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> TransformComponent;
	
	UPROPERTY()
	class ABrickBoardManager* OwnerBoardManager;
	
	EDropStartDirection MyStartDirection;	//떨어지는 곳. 사분면.	
	FVector2D BasePoint;					//브릭의 위치의 기준이 되는 좌표. 브릭의 좌상단임.
	BrickType MyBrickType;					//현재 브릭 모양
	BrickDirection MyBrickDirection;		//현재 브릭 방향
	vector<vector<UnitSkin>> Brick;			//Unit 위치 및 모양 데이터 구성

	/** DropBrick을 구성하는 실제 유닛 */
	vector<vector<class AEMUnitBrick*>> DropBrickUnits;
};
