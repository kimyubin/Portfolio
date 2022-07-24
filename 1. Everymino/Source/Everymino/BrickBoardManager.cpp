// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BrickBoardManager.h"

#include <random>

#include "EMDropBrick.h"
#include "EMGameInstance.h"
#include "EMGamePlayStatics.h"
#include "EMPlayData.h"
#include "EMUnitBrick.h"

#include "Everymino.h"

ABrickBoardManager::ABrickBoardManager()
{
	PrimaryActorTick.bCanEverTick = true;
	TransformComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TransformComp"));
	RootComponent = TransformComponent;
	HoldBrickType = BrickType::None;
	PressInputBS.reset();
	CurrentControlZone = EDropStartDirection::North;
}

// Called when the game starts or when spawned
void ABrickBoardManager::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerDataPtr = Cast<UEMGameInstance>(GetGameInstance())->GetPlayerData();
	PlayerDataPtr->ChangeUnitSkin(BrickType::L, 3,UnitSkin::Skin);

	InitSpawnBoard();
	DropBricks = vector<AEMDropBrick*>(EnumToInt(EDropStartDirection::Size), nullptr);
	
	for (int i = 0; i < DropBricks.size(); ++i)
	{		
		DropBricks[i] = GetWorld()->SpawnActor<AEMDropBrick>(AEMDropBrick::StaticClass(),FVector::ZeroVector,FRotator::ZeroRotator);
		
		DropBricks[i]->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		DropBricks[i]->InitBrick(this, static_cast<EDropStartDirection>(i));
	}
	//선택된 사분면 정보전달.
	DropBricks[EnumToInt(CurrentControlZone)]->SetIsSelected(true);
}

void ABrickBoardManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//상하 좌우 이동만 제어
	if (PressInputBS.any())	
		DropBricks[EnumToInt(CurrentControlZone)]->PassInput(PressInputBS);	

	LineClearCheck();
}

void ABrickBoardManager::InitSpawnBoard()
{
	auto GBoard = PlayerDataPtr->GetCurrentGameBoard();
	int MaxY = PlayerDataPtr->GetMaxBoardY();
	int MaxX = PlayerDataPtr->GetMaxBoardX();

	//유닛 1개가 이미 센터에 위치하기 때문에 1개 빼줌.
	SetActorLocation(FVector(- BrickTemplate::UNIT_SIZE*(MaxX-1)/2,0.f,BrickTemplate::UNIT_SIZE*(MaxY-1)/2));
	if(GameBoardData.size()==0)
		GameBoardData = GBoard;
	
	for (int y = 0; y < MaxY; ++y)
	{
		GameBoardUnits.push_back(vector<AEMUnitBrick*>());
		for (int x = 0; x < MaxX; ++x)
		{
			GameBoardUnits.back().push_back(UEMGamePlayStatics::SpawnUnit(x, y, GameBoardData[y][x], this));
		}
	}
}
void ABrickBoardManager::RebuildBoard()
{
	for(int y = 0; y < GameBoardUnits.size(); ++y)
	{
		for(int x = 0; x < GameBoardUnits[y].size();++x)
		{
			UnitSkin NextUnitSkin = GameBoardData[y][x];
			UnitSkin NowUnitSkin = UnitSkin::Empty;

			if (GameBoardUnits[y][x] != nullptr)
				NowUnitSkin = GameBoardUnits[y][x]->MySkinNum;
			
			if(NowUnitSkin == NextUnitSkin)
				continue;

			if (NowUnitSkin == UnitSkin::Empty) //&& if (NextUnitSkin != UnitSkin::Empty)
			{
				GameBoardUnits[y][x] = UEMGamePlayStatics::SpawnUnit(x, y, NextUnitSkin, this);
			}
			else // if (NowUnitSkin != UnitSkin::Empty) 
			{
				if (NextUnitSkin == UnitSkin::Empty)				
					UEMGamePlayStatics::DestroyUnit(&GameBoardUnits[y][x]);				
				else
					GameBoardUnits[y][x]->SetSkinMesh(NextUnitSkin);
			}			
		}
	}
}
bool ABrickBoardManager::CheckBoard(AEMDropBrick* InDropBrick)
{
	//각 드롭브릭이랑 검사도 추가해야함.
	const vector<vector<UnitSkin>> InBrick = InDropBrick->Brick;

	//중앙판 벽 위치
	const uint16 CenterNorthBorder = PlayerDataPtr->GetFrontCenterPoint();
	const uint16 CenterEastBorder = PlayerDataPtr->GetBackCenterPoint();
	const uint16 CenterSouthBorder = PlayerDataPtr->GetBackCenterPoint();
	const uint16 CenterWestBorder = PlayerDataPtr->GetFrontCenterPoint();
	
	for(int dby = 0; dby < InBrick.size();++dby)
	{
		for(int dbx = 0; dbx < InBrick[dby].size();++dbx)
		{			
			//드롭 유닛이 존재하면
			if (InBrick[dby][dbx] != UnitSkin::Empty)
			{
				const FVector2D DropBrickPos = InDropBrick->GetUnitLocation(dbx,dby);
				//경계 검사.
				if (DropBrickPos.Y >= GameBoardData.size()
					|| DropBrickPos.X >= GameBoardData.front().size()
					|| DropBrickPos.Y < 0
					|| DropBrickPos.X < 0)
					return false;
				//중앙판 경계 검사
				//각 경계는 해당 사면에서 출발하는 브릭만 통과할 수 있음
				if (InDropBrick->MyStartDirection != EDropStartDirection::North
					&& DropBrickPos.Y <= CenterNorthBorder)
					return false;
				if (InDropBrick->MyStartDirection != EDropStartDirection::East
					&& DropBrickPos.X >= CenterEastBorder)
					return false;
				if (InDropBrick->MyStartDirection != EDropStartDirection::South
					&& DropBrickPos.Y >= CenterSouthBorder)
					return false;
				if (InDropBrick->MyStartDirection != EDropStartDirection::West
					&& DropBrickPos.X <= CenterWestBorder)
					return false;

				// 유닛 겹침 검사.
				if (GameBoardData[DropBrickPos.Y][DropBrickPos.X] != UnitSkin::Empty)
					return false;
			}
		}
	}
	return true;
}

void ABrickBoardManager::AttachDropBrick(AEMDropBrick* InDropBrick)
{
	for(int dby = 0; dby < InDropBrick->Brick.size();++dby)
	{
		for(int dbx = 0; dbx < InDropBrick->Brick[dby].size();++dbx)
		{			
			if (InDropBrick->GetUnitSkin(dbx, dby) != UnitSkin::Empty)
			{
				FVector2D DropBrickPos = InDropBrick->GetUnitLocation(dbx,dby);
				GameBoardData[DropBrickPos.Y][DropBrickPos.X] = InDropBrick->Brick[dby][dbx];
				GameBoardUnits[DropBrickPos.Y][DropBrickPos.X] = InDropBrick->DropBrickUnits[dby][dbx];
				GameBoardUnits[DropBrickPos.Y][DropBrickPos.X]->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				InDropBrick->DropBrickUnits[dby][dbx] = nullptr;
			}
		}
	}
	InDropBrick->NewBrick();
}

pair<bool, int> ABrickBoardManager::LineClearCheck()
{
	int LastClearFloor = 0;
	//todo 1. 각축 별로 라인 클리어 체크
	//todo 2. 중심 축
	//천장부터 바닥 직전까지 훑음.
	
	for(int gby = 0; gby < GameBoardData.size()-1; ++gby)
	{
		auto it = find(GameBoardData[gby].begin(), GameBoardData[gby].end(), UnitSkin::Empty);
		//한줄 전체에 빈공간이 없음. == 꽉참
		if (it == GameBoardData[gby].end())
		{
			//클리어층 바로 위칸부터 한칸씩 내리기. 0번은 천장이라 따로 계산
			for (int cgby = gby; cgby > LastClearFloor; --cgby)
			{
				//좌우 벽만 빼고 복사. 벽 스킨 때문에 통으로 내리는게 아니라 중간만 복사
				for (int x = 1; x < GameBoardData[cgby].size() - 1; ++x)
					GameBoardData[cgby][x] = GameBoardData[cgby - 1][x];
			}
			if(LastClearFloor == 0)
				GameBoardData[0] = PlayerDataPtr->GetCurrentGameBoard()[0];

			//이미 내려서 확정적으로 빈칸인 곳 바로 아래는 위칸이 빈칸이므로
			//그냥 기본 정보를 복사해서 가져옴
			//ex) 1줄 삭제 -> 전체 1칸 내리니까, 0번(맨위)은 무조건 빈칸 -> 0번은 복사
			//ex) 2줄 삭제 -> 전체 2칸 내리니까, 0번, 1번은 빈칸 확정 + 0번은 이미 빈칸 -> 1번만 복사
			GameBoardData[LastClearFloor] = PlayerDataPtr->GetCurrentGameBoard()[LastClearFloor];
			LastClearFloor++;
		}		
	}

	if(LastClearFloor > 0)
	{
		RebuildBoard();
		return pair<bool, int>(true, LastClearFloor);		
	}
	
	return pair<bool, int>(false, 0);
	
}

void ABrickBoardManager::SetPressInput(EMInput InInput)
{
	PressInputBS.set(EnumToInt(InInput));
}

void ABrickBoardManager::SetReleasedInput(EMInput InInput)
{
	PressInputBS.reset(EnumToInt(InInput));
	
	switch (InInput)
	{
	case EMInput::LeftMove:
	case EMInput::RightMove:
	case EMInput::UpMove:
	case EMInput::DownMove:
		DropBricks[EnumToInt(CurrentControlZone)]->SetMoveButtonRelease();
		break;
	default:break;
	}	
}

void ABrickBoardManager::PressedRotateInput(EMInput InInput)
{
	DropBricks[EnumToInt(CurrentControlZone)]->SpinDropBrick(InInput);
}
void ABrickBoardManager::PressedHardDrop()
{
	DropBricks[EnumToInt(CurrentControlZone)]->MoveHardDrop();
}

void ABrickBoardManager::SetCurrentControlZone(EDropStartDirection InZone)
{
	//선택된 사분면 변경 및 해당 브릭에 정보 전달.
	DropBricks[EnumToInt(CurrentControlZone)]->SetIsSelected(false);
	CurrentControlZone = InZone;
	DropBricks[EnumToInt(CurrentControlZone)]->SetIsSelected(true);
}

AEMPlayData* ABrickBoardManager::GetPlayerDataPtr()
{
	return PlayerDataPtr;
}

