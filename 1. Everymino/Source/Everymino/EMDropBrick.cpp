// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EMDropBrick.h"
#include "EMUnitBrick.h"

#include "BrickBoardManager.h"
#include "EMGamePlayStatics.h"
#include "EMPlayData.h"

AEMDropBrick::AEMDropBrick()
{
	PrimaryActorTick.bCanEverTick = true;
	TransformComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TransformComp"));
	RootComponent = TransformComponent;

	OwnerBoardManager = nullptr;
	BasePoint = FVector2D();
	MyBrickType = BrickType::T;
	MyBrickDirection = BrickDirection::UP;
	Brick = vector<vector<UnitSkin>>();
}

void AEMDropBrick::BeginPlay()
{
	Super::BeginPlay();
}

void AEMDropBrick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEMDropBrick::InitBrick(ABrickBoardManager* InOwnerBoardManager, EDropStartDirection InStartDirection)
{
	OwnerBoardManager = InOwnerBoardManager;
	MyStartDirection = InStartDirection;
	MyBrickType = OwnerBoardManager->GetPlayerDataPtr()->GetNewBrickType();

	switch (MyStartDirection)
	{
	case EDropStartDirection::North: MyBrickDirection = BrickDirection::UP;
		break;
	case EDropStartDirection::East: MyBrickDirection = BrickDirection::RIGHT;
		break;
	case EDropStartDirection::South: MyBrickDirection = BrickDirection::DOWN;
		break;
	case EDropStartDirection::West: MyBrickDirection = BrickDirection::LEFT;
		break;
	default: break;
	}

	BasePoint = OwnerBoardManager->GetPlayerDataPtr()->GetDropStartPoint(MyStartDirection);
	Brick = OwnerBoardManager->GetPlayerDataPtr()->GetBrickShape(MyBrickType, MyBrickDirection);

	//유닛 스폰 및 저장.	
	for (int dpY = 0; dpY < Brick.size(); ++dpY)
	{
		DropBrickUnits.push_back(vector<AEMUnitBrick*>());

		for (int dpX = 0; dpX < Brick[dpY].size(); ++dpX)
		{
			UnitSkin DataSkin = GetUnitSkin(dpX, dpY);
			FVector2D UnitLoc = GetUnitLocation(dpX, dpY);
			DropBrickUnits.back().push_back(UEMGamePlayStatics::SpawnUnit(UnitLoc, DataSkin, this));
		}
	}
}

void AEMDropBrick::NewBrick()
{
	BasePoint = OwnerBoardManager->GetPlayerDataPtr()->GetDropStartPoint(MyStartDirection);
	MyBrickType = OwnerBoardManager->GetPlayerDataPtr()->GetNewBrickType();

	switch (MyStartDirection)
	{
	case EDropStartDirection::North: MyBrickDirection = BrickDirection::UP;
		break;
	case EDropStartDirection::East: MyBrickDirection = BrickDirection::RIGHT;
		break;
	case EDropStartDirection::South: MyBrickDirection = BrickDirection::DOWN;
		break;
	case EDropStartDirection::West: MyBrickDirection = BrickDirection::LEFT;
		break;
	default: break;
	}
	Brick = OwnerBoardManager->GetPlayerDataPtr()->GetBrickShape(MyBrickType, MyBrickDirection);
	UpdateDroppingBrick();
}

void AEMDropBrick::InputHandler(EMInput InInput)
{
	switch (InInput)
	{
	case EMInput::AntiClockwise:
	case EMInput::Clockwise:
		break;
	case EMInput::LeftMove:
	case EMInput::RightMove:
		break;
	case EMInput::SoftDrop:
	case EMInput::HardDrop:
		break;
	default:
		break;
	}
}

void AEMDropBrick::SpinBrick(BrickDirection Dir)
{
	MyBrickDirection = Dir;
	Brick = OwnerBoardManager->GetPlayerDataPtr()->GetBrickShape(MyBrickType, MyBrickDirection);
}

void AEMDropBrick::SpinBrick(EMInput SpinInput)
{
	int tempDir = EnumToInt(MyBrickDirection);
	int MaxSize = EnumToInt(BrickDirection::Size);

	if (SpinInput == EMInput::Clockwise)
		tempDir = (tempDir + 1) % MaxSize;
	else if (SpinInput == EMInput::AntiClockwise) //-1,0,1,2에 4를 더하고 나머지 연산하면 3,0,1,2
		tempDir = ((tempDir - 1) + MaxSize) % MaxSize;
	else
		return;

	SpinBrick(static_cast<BrickDirection>(tempDir));
}

void AEMDropBrick::UpdateDroppingBrick()
{
	for (int dpY = 0; dpY < DropBrickUnits.size(); ++dpY)
	{
		for (int dpX = 0; dpX < DropBrickUnits[dpY].size(); ++dpX)
		{
			UnitSkin DataSkin = GetUnitSkin(dpX, dpY);
			FVector2D UnitLoc = GetUnitLocation(dpX, dpY);

			//실제 유닛있는데, 비어야하면 파괴.
			//있어야하면, 이동 후 스킨 변경
			if (DropBrickUnits[dpY][dpX] != nullptr)
			{
				if (DataSkin == UnitSkin::Empty)
					UEMGamePlayStatics::DestroyUnit(&DropBrickUnits[dpY][dpX]);
				else
					DropBrickUnits[dpY][dpX]->SetPositionAndSkin(UnitLoc.X, UnitLoc.Y, DataSkin);
			}
			else if (DataSkin != UnitSkin::Empty)
			{
				//실제 유닛 없는데, 데이터가 있으면 스폰
				DropBrickUnits[dpY][dpX] = UEMGamePlayStatics::SpawnUnit(UnitLoc, DataSkin, this);
			}
		}
	}
}

void AEMDropBrick::SpinDroppingBrick(EMInput InInput)
{
	//실제 브릭 회전하는 함수.

	SpinBrick(InInput);

	if (CheckBoard())
	{
		UpdateDroppingBrick();
		return;
	}

	//회전 실패시 이동 후 검증.
	FVector2D prevBasePoint = BasePoint;
	for (int tryNum = 0; tryNum < 5; ++tryNum)
	{
		BasePoint += BrickTemplate::GetWallKickData(MyBrickType, MyBrickDirection, InInput, tryNum, MyStartDirection);
		if (CheckBoard())
		{
			UpdateDroppingBrick();
			return;
		}
		BasePoint = prevBasePoint;
	}
	//모두 실패시 회전 원상복귀
	if(InInput==EMInput::Clockwise)
		SpinBrick(EMInput::AntiClockwise);
	else
		SpinBrick(EMInput::Clockwise);
}

void AEMDropBrick::MoveDroppingBrick(EMInput InInput)
{
	auto PrvBasePoint = BasePoint;

	switch (InInput)
	{
	case EMInput::LeftMove:
	case EMInput::RightMove:
	case EMInput::UpMove:
	case EMInput::DownMove:
		MoveBasePoint(InInput);
		break;
	case EMInput::HardDrop:
		while (CheckBoard())
		{
			BasePoint.Y++;
		}
		BasePoint.Y--;
		MoveBrickUnits();
	//SetReleasedInput(EMInput::HardDrop);
		return;
		break;
	default:
		break;
	};

	if (CheckBoard())
		MoveBrickUnits();
	else
		BasePoint = PrvBasePoint;
}

void AEMDropBrick::MoveBasePoint(EMInput InInput)
{
	auto PrvBasePoint = BasePoint;

	switch (InInput)
	{
	case EMInput::UpMove:
		if (MyStartDirection != EDropStartDirection::North)
			BasePoint.Y--;
		break;
	case EMInput::DownMove:
		if (MyStartDirection != EDropStartDirection::South)
			BasePoint.Y++;
		break;
	case EMInput::RightMove:
		if (MyStartDirection != EDropStartDirection::East)
			BasePoint.X++;
		break;
	case EMInput::LeftMove:
		if (MyStartDirection != EDropStartDirection::West)
			BasePoint.X--;
		break;
	default:
		break;
	};
	// if(CheckBoard())	
	// 	MoveBrickUnits();
	// else
	if (!CheckBoard())
		BasePoint = PrvBasePoint;
}

void AEMDropBrick::MoveSoftDrop()
{
	EMInput Temp = EMInput::None;
	switch (MyStartDirection)
	{
	case EDropStartDirection::North:
		Temp = EMInput::DownMove;
		break;
	case EDropStartDirection::East:
		Temp = EMInput::LeftMove;
		break;
	case EDropStartDirection::South:
		Temp = EMInput::UpMove;
		break;
	case EDropStartDirection::West:
		Temp = EMInput::RightMove;
		break;
	default:
		break;
	}
	MoveBasePoint(Temp);
	MoveBrickUnits();
}

void AEMDropBrick::MoveHardDrop()
{
	while (CheckBoard())
	{
		MoveSoftDrop();
	}
	MoveBrickUnits();
}

void AEMDropBrick::MoveBrickUnits()
{
	for (int dpY = 0; dpY < DropBrickUnits.size(); ++dpY)
	{
		for (int dpX = 0; dpX < DropBrickUnits[dpY].size(); ++dpX)
		{
			if (DropBrickUnits[dpY][dpX] != nullptr)
			{
				FVector2D UnitLoc = GetUnitLocation(dpX, dpY);
				DropBrickUnits[dpY][dpX]->SetPosition(UnitLoc.X, UnitLoc.Y);
			}
		}
	}
}

bool AEMDropBrick::CheckBoard()
{
	return OwnerBoardManager->CheckBoard(this);
}

FVector2D AEMDropBrick::GetUnitLocation(int InX, int InY) const
{
	return FVector2D(InX + BasePoint.X, InY + BasePoint.Y);
}
