// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BrickTemplate.h"
#include "GameFramework/Actor.h"
#include "EMPlayData.generated.h"

using namespace std;

USTRUCT(BlueprintType)
struct FUnitSkinData
{
	GENERATED_USTRUCT_BODY()
	FUnitSkinData(UnitSkin BT=UnitSkin::Default):BrickUnitSkin(BT), StaticMesh(nullptr){}

	UPROPERTY(VisibleAnywhere)
	UnitSkin BrickUnitSkin;
	UPROPERTY(EditAnywhere)
	UStaticMesh* StaticMesh;	
};

/**
 * 본 클래스는 자식 BP를 통해서만 스폰되어야함.
 * 플레이시 필요한 종합 데이터 접근용. 저장도 추가해야함
 * 블루프린트에서 수정해서 사용하는 변수들 보아놓음
 * 브릭 스킨 목록,  게임보드판 모양 등등
 */
UCLASS()
class EVERYMINO_API AEMPlayData : public AActor
{
	GENERATED_BODY()
	
public:	
	AEMPlayData();

protected:
	virtual void BeginPlay() override;

public:	
	/** 특정 brick의 방향 반환 */
	vector<vector<UnitSkin>> GetBrickShape(BrickType BrickT, BrickDirection Dir);
	
	/** 현재 진행중인 게임보드 정보 */
	vector<vector<UnitSkin>> GetCurrentGameBoard();
	
	/** 게임보드 데이터를 만들어서 불러옴. 이전에 만든 게임보드는 해쉬맵에 저장했다가 불러옴. */ 
	vector<vector<UnitSkin>> GetNewGameBoard(uint16 InCenterWidth, uint16 InShaftWidth, uint16 InShaftHeight);
private:
	/**
	 * 게임보드 자동 생성. 둘다 홀수거나 짝수여야함. 아니면 total을 1 늘림.
	 * 해쉬맵을 위해 부호없는 16비트 정수형
	 * @param InCenterWidth 가운데 교차지점 한변의 길이 
	 * @param InShaftWidth 기둥 너비
	 * @param InShaftHeight 기둥 높이 
	 */
	vector<vector<UnitSkin>> GenerateGameBoard(uint16 InCenterWidth, uint16 InShaftWidth, uint16 InShaftHeight);

public:
	/** 모든 브릭 스킨 변경 */
	void ChangeAllBricksSkin(UnitSkin Skin);
	
	/** 특정 브릭만 스킨 변경 */
	void ChangeBrickSkin(BrickType TargetBrickT, UnitSkin Skin);
	
	/** 특정 브릭의 특정 유닛만 스킨 변경 */
	void ChangeUnitSkin(BrickType TargetBrickT, uint8 TargetUnitNum, UnitSkin Skin);
	
	/**
	 * 조작 키 변경, 적용 및 키 셋팅 저장
	 * @param TargetBindControl 바꾸고 싶은 조작 대상의 열거형
	 * @param InputKey 실제 누르는 입력키 EKeys::
	 */
	void ChangeSaveKeySetting(EBindKey::EControlName TargetBindControl, FKey InputKey);

	/** 저장된 키 셋팅 불러오기*/	
	void LoadKeySettings();
	
	/** 스킨용 매쉬 반환 */
	UStaticMesh* GetUnitSkinMesh(UnitSkin SkinNum);

	/** 새로운 브릭 생성. 14bag으로 셔플 */
	BrickType GetNewBrickType();

	/** getter */
	uint8 GetMaxBoardX() const { return CustomGameBoard.front().size(); }
	uint8 GetMaxBoardY() const { return CustomGameBoard.size(); }
	FVector2D GetDropStartPoint(EDropStartDirection InStartDirection);	//드롭 브릭 시작 지점.
	float GetDelayedAutoShift() const { return DelayedAutoShift; }
	float GetAutoRepeatRate() const { return AutoRepeatRate; }
	float GetAutoSoftDropTime() const { return AutoSoftDropTime; }
	float GetLockdownDelay() const { return LockdownDelay; }
	uint8 GetLockdownCount() const { return LockdownCount; }
	uint8 GetAerialLockdownCount() const { return AerialLockdownCount; }	
	
private:

	/** 브릭용 스킨 목록. BP에서 추가. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Skins")	
	TArray<FUnitSkinData> UnitSkinMeshes;
	
	/** 각 브릭 방향별 스킨 정보 */
	vector<vector<vector<vector<UnitSkin>>>> MyBricksSkinData;
	
	/** 게임보드 크기에 따른 게임보드 모양 */
	vector<vector<UnitSkin>> CustomGameBoard;
	
	/** 한번 불러왔던 게임보드는 다시 만들지 않고 저장했다가 불러오는 용. CenterWidth, ShaftWidth, ShaftHeight순서. */
	unordered_map<uint64, vector<vector<UnitSkin>>> CustomGameBoardMap;		

	/** Brick 순서 담는 가방. 선입선출*/
	deque<BrickType> BrickBag;


	/** 중앙판 한변의 크기. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "GameBoard")
	uint16 CenterWidth;
	
	/** 기둥의 너비.  CenterWidth보다 작거나 같아야함. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "GameBoard")
	uint16 ShaftWidth;
	
	/** 기둥 높이. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "GameBoard")
	uint16 ShaftHeight;
	
	/** 전체 게임판 한 변의 크기. ShaftHeight*2+CenterWidth로 계산됨. */
	uint16 GameBoardWidth;
	
	/** 상하 좌우 여백 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "GameBoard")
	uint16 VanishZone;


	
	////기준 시간
	
	/** 자동 이동 발생까지 기준 시간. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "RefTime")
	float DelayedAutoShift;
	/** 이동 발생 후 브릭이 1칸 움직일때 사용하는 시간. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "RefTime")	
	float AutoRepeatRate;
	/** 1칸 내려가는 시간 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "RefTime")	
	float AutoSoftDropTime;
	/** 비비기 할때 강제로 고정되는 시간 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "RefTime")	
	float LockdownDelay;
	/** 비비기 할때 강제로 고정되는 카운터 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "RefTime")	
	uint8 LockdownCount;
	/** 공중 비비기할때 강제로 고정되는 시간 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "RefTime")	
	uint8 AerialLockdownCount;
	
	
};
