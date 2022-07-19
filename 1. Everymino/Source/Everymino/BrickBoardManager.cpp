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
	CumulativeTime = 0.f;
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
	
}

void ABrickBoardManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//오버플로우 방지
	CumulativeTime = FMath::Clamp(CumulativeTime + DeltaTime, 0.f, PlayerDataPtr->GetDelayedAutoShift() + 1.f);

	LockDownCheck();

	//todo 입력을 각 브릭으로 직접 토스해서, 브릭 tick에서 직접 컨트롤하게 변경. 
	if (CumulativeTime >= PlayerDataPtr->GetDelayedAutoShift())
	{
		if(PressInputBS.any())
		{
			//동시 입력계산을 위해 비트셋 사용
			if (PressInputBS.test(EnumToInt(EMInput::AntiClockwise)))
				DropBricks[EnumToInt(CurrentControlZone)]->PassInput(EMInput::AntiClockwise);
			if (PressInputBS.test(EnumToInt(EMInput::Clockwise)))
				DropBricks[EnumToInt(CurrentControlZone)]->PassInput(EMInput::Clockwise);

			if (PressInputBS.test(EnumToInt(EMInput::LeftMove)))
				DropBricks[EnumToInt(CurrentControlZone)]->PassInput(EMInput::LeftMove);
			if (PressInputBS.test(EnumToInt(EMInput::RightMove)))
				DropBricks[EnumToInt(CurrentControlZone)]->PassInput(EMInput::RightMove);
			if (PressInputBS.test(EnumToInt(EMInput::UpMove)))
				DropBricks[EnumToInt(CurrentControlZone)]->PassInput(EMInput::UpMove);
			if (PressInputBS.test(EnumToInt(EMInput::DownMove)))
				DropBricks[EnumToInt(CurrentControlZone)]->PassInput(EMInput::DownMove);
			
			CumulativeTime = 0.f;
		}
		//PressInputBS.any() or 소프트 드롭 직후에만 계산.
	}
	
	if (PressInputBS.test(EnumToInt(EMInput::HardDrop)))
		DropBricks[EnumToInt(CurrentControlZone)]->MoveDropBrick(EMInput::HardDrop);

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
	for(int dby = 0; dby < InBrick.size();++dby)
	{
		for(int dbx = 0; dbx < InBrick[dby].size();++dbx)
		{			
			//드롭 유닛이 존재하면
			if (InBrick[dby][dbx] != UnitSkin::Empty)
			{
				FVector2D DropBrickPos = InDropBrick->GetUnitLocation(dbx,dby);
				//경계 검사.
				if (DropBrickPos.Y >= GameBoardData.size()
					|| DropBrickPos.X >= GameBoardData.front().size()
					|| DropBrickPos.Y < 0
					|| DropBrickPos.X < 0)
						return false;
				// 유닛 겹침 검사.
				if (GameBoardData[DropBrickPos.Y][DropBrickPos.X] != UnitSkin::Empty)
					return false;
			}
		}
	}
	return true;
}

bool ABrickBoardManager::LockDownCheck()
{
	//내려갈 칸이 있는지 확인.
	DropBricks[0]->BasePoint.Y++;
		
	bool isMoreDown = CheckBoard(DropBricks[0]);	
	DropBricks[0]->BasePoint.Y--;
	
	//더이상 내려갈 수 없음
	if(!isMoreDown)
	{
		//브릭을 게임판에 부착. 기존 유닛 재사용.
		for(int dby = 0; dby < DropBricks[0]->Brick.size();++dby)
		{
			for(int dbx = 0; dbx < DropBricks[0]->Brick[dby].size();++dbx)
			{			
				if (DropBricks[0]->GetUnitSkin(dbx, dby) != UnitSkin::Empty)
				{
					FVector2D DropBrickPos = DropBricks[0]->GetUnitLocation(dbx,dby);
					GameBoardData[DropBrickPos.Y][DropBrickPos.X] = DropBricks[0]->Brick[dby][dbx];
					GameBoardUnits[DropBrickPos.Y][DropBrickPos.X] = DropBricks[0]->DropBrickUnits[dby][dbx];
					GameBoardUnits[DropBrickPos.Y][DropBrickPos.X]->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
					DropBricks[0]->DropBrickUnits[dby][dbx] = nullptr;
				}
			}
		}
		//RebuildBoard();
		DropBricks[0]->NewBrick();
	}
	
	return !isMoreDown;
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
void ABrickBoardManager::DrawGhostBrick()
{
	// auto ghostBrick = DropBrickData;
	// while(CheckBoard())
	// {
	// 	ghostBrick.BasePoint.Y++;			
	// }
	// ghostBrick.BasePoint.Y--;
	
}