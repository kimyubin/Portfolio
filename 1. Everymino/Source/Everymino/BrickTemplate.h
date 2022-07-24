// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <deque>

#include "CoreMinimal.h"

#define EnumToInt(EN) static_cast<uint8>(EN)
//UnitSkin::Empty
#define US_E UnitSkin::Empty
//UnitSkin::Wall
#define US_W UnitSkin::Wall
//UnitSkin::Default
#define US_D UnitSkin::Default

enum class BrickType:uint8;
enum class BrickDirection:uint8;
enum class UnitSkin:uint8;
enum class EMInput:uint8;
enum class EDropStartDirection:uint8;
/**
 * 브릭 좌상단 위치, 모양, 방향, 각 유닛의 상대 위치 보관
 */
struct BrickInfo;

using namespace std;
/**
 * 기본 브릭 형태 저장하는 클래스
 * 용어정리							<p>
 * Unit - 1칸짜리 블럭. 벽돌.			<p>
 * Brick - Mino. 유닛이 모여 만든 블럭. I,sq,t,s,z,j,L이 있음<p>
 * BrickShape - 블럭의 형상. 모양		<p>
 * UnitSkin - 1칸짜리 블럭 스킨		<p>
 */
class EVERYMINO_API BrickTemplate
{
public:
	//유닛 최대 개수. 0부터 계산. 4개면 0~3.
	static constexpr uint8 MAX_UNIT_NUM = 4;	
	static constexpr uint8 UNIT_SIZE = 50;		//한칸 사이즈 50

	BrickTemplate();
	~BrickTemplate();
	static vector<vector<vector<vector<UnitSkin>>>> GetBrickShapes(){return BrickShapes;}
	static deque<BrickType> GetShuffleBag(){return ShuffleBag;}
	static FVector2D GetWallKickData(BrickType InBrickType, BrickDirection InDirection,EMInput Rotation ,uint8 TryNum, EDropStartDirection InStartDirection);
	
	
private:
	static vector<vector<vector<vector<UnitSkin>>>> BrickShapes;
	static deque<BrickType> ShuffleBag;
	static vector<vector<FVector2D>> WallKickData;
	static vector<vector<FVector2D>> I_WallKickData;
	
};

/** Brick의 모양. I,Sq,T,S,Z,J,L None이 있음. 템플릿 배열 번호라서 순서는 변경할 수 없음.	*/
enum class BrickType:uint8
{	
	I,
	Sq,
	T,
	S,
	Z,
	J,
	L,
	None,
	
	Size
};
/** Brick의 방향. 상>우>하>좌 순.			*/
enum class BrickDirection:uint8
{
	UP,
	RIGHT,
	DOWN,
	LEFT,
	
	Size
};

/** 유닛의 스킨. empty는 빈칸임.			*/
UENUM(BlueprintType)
enum class UnitSkin:uint8
{	
	Empty,
	Default,
	Skin,
	
	Wall,
	InvisibleWall,	//겹침 체크용 투명 벽. 벽과 게임판 외부 공간을 채워서 체크함 

	Size
};

/** 입력 Brick 이동, 회전 방향 */
enum class EMInput:uint8
{
	None,
	AntiClockwise,	//Left
	Clockwise,		//Right
	SoftDrop,
	HardDrop,
	
	UpMove,
	DownMove,
	LeftMove,
	RightMove,
	
	Size
};

/** 드롭 시작 위치 지정. 북쪽부터 시계방향*/
UENUM()
enum class EDropStartDirection:uint8
{
	North,	//북, 상
	East,	//동, 우
	South,	//남, 하
	West,	//서, 좌

	Size
};

//런타임 키변경용 바인딩 키
struct EBindKey
{
	static FKey MoveUp;
	static FKey MoveRight;
	static FKey MoveDown;
	static FKey MoveLeft;

	static FKey HardDrop;
	
	static FKey Clockwise;
	static FKey AntiClockwise;
	
	static FKey SelectNorth;
	static FKey SelectEast;
	static FKey SelectSouth;
	static FKey SelectWest;

	/** 현재 모든 키 설정 리턴*/
	static TArray<FKey> GetKeys()
	{		
		return{
				MoveUp,
				MoveRight,
				MoveDown,
				MoveLeft,

				HardDrop,
			
				Clockwise,
				AntiClockwise,
			
				SelectNorth,
				SelectEast,
				SelectSouth,
				SelectWest,
				};
	}
	/** 모든 설정 한번에 적용*/
	static void SetKeys(const TArray<FKey> &InKeys)
	{
		if(InKeys.Num()!= EnumToInt(EControlName::Size))
			return;
		MoveUp = InKeys[EnumToInt(EControlName::MoveUp)];
		MoveRight = InKeys[EnumToInt(EControlName::MoveRight)];
		MoveDown = InKeys[EnumToInt(EControlName::MoveDown)];
		MoveLeft = InKeys[EnumToInt(EControlName::MoveLeft)];

		HardDrop = InKeys[EnumToInt(EControlName::HardDrop)];
		
		Clockwise = InKeys[EnumToInt(EControlName::Clockwise)];
		AntiClockwise = InKeys[EnumToInt(EControlName::AntiClockwise)];
		
		SelectNorth = InKeys[EnumToInt(EControlName::SelectNorth)];
		SelectEast = InKeys[EnumToInt(EControlName::SelectEast)];
		SelectSouth = InKeys[EnumToInt(EControlName::SelectSouth)];
		SelectWest = InKeys[EnumToInt(EControlName::SelectWest)];
	}
	
	enum class EControlName
	{
		
		MoveUp,
		MoveRight,
		MoveDown,
		MoveLeft,

		HardDrop,
			
		Clockwise,
		AntiClockwise,
			
		SelectNorth,
		SelectEast,
		SelectSouth,
		SelectWest,

		Size
	};
};