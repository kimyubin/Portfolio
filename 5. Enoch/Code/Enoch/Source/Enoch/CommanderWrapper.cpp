// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CommanderWrapper.h"

#include <map>
#include <random>
#include <unordered_map>

#include "EnochField.h"
#include "EnochFieldCell.h"
#include "EnochFreeLancer.h"
#include "Kismet/GameplayStatics.h"

UCommanderWrapper::UCommanderWrapper()
{
	
}

void UCommanderWrapper::InitCommander()
{
	commander = EnochCommander();
}

void UCommanderWrapper::SetCommander(EnochCommander* _commander)
{
	commander = *_commander;
}

uint8 UCommanderWrapper::addAbilityPoint(int add)
{
	commander.abilityPoint += add;
	return commander.abilityPoint;
}

int32 UCommanderWrapper::addGold(int add)
{
	commander.gold += add;
	return commander.gold;
}

FString UCommanderWrapper::GetLevelString()
{
	return FString::Printf(TEXT("Lv.%d"), commander.level);
}

FString UCommanderWrapper::GetExpString()
{
	return FString::Printf(TEXT("%d/%d")
		, commander.exp
		, EnochCommander::getExpForNextLevel(commander.level));
}

uint8 UCommanderWrapper::addExp(int add)
{
	if( add <= 0 || commander.gold < add || commander.level >= EnochCommander::LEVEL_MAX)
		return commander.exp;
	commander.gold -= add;
	return commander.addExp(add);
}
bool UCommanderWrapper::ReqUserInfo()
{
	ReqNewRecruitList();
	ReqNewEnemyPool();
	return true;
}
bool UCommanderWrapper::ReqNewRecruitList()
{
	commander.RecruitFreeLancerList = RandomRecruitListTemp();
	//합성 가능 목록으로 상점 목록 업데이트	
	commander.CombineRecruitList();
	for (int i = EnochFieldData::GetHeight()/2; i < EnochFieldData::GetHeight(); i++)
	{
		for (int j = 0; j < EnochFieldData::GetWidth(); j++)
		{			
			if(i%2==0 && j %3==0)
			{
				//적 필드 초기화
				commander.FieldFreeLancerList[i][j] = FreeLancerSaveData(FreeLancerTemplateID::Melee2,1);
			}			
		}
	}
	return true;
}
bool UCommanderWrapper::ReqNewEnemyPool()
{
	// 3개풀 * 9명 * 적필드
	commander.EnemyPoolFieldList.clear();	
	std::vector<FreeLancerSaveData> Result(commander.MAX_RECRUITSLOT, FreeLancerSaveData());
	std::random_device deviceSeed;
	std::mt19937 rdEndgine(deviceSeed());
	std::uniform_int_distribution<int> rdFLRange(static_cast<int>(FreeLancerTemplateID::Melee1), static_cast<int>(FreeLancerTemplateID::Melee2));
	std::uniform_int_distribution<int> rdPersentRange(1,10);

	//대충 랜덤으로 생성하는 거라 난잡함.
	commander.EnemyPoolFieldList =
		vector<vector<vector<vector<FreeLancerSaveData>>>>
		(3,vector<vector<vector<FreeLancerSaveData>>>
			(9,vector<vector<FreeLancerSaveData>>
				(5,vector<FreeLancerSaveData>
					(10,FreeLancerSaveData()))));
	for(int pool = 0; pool<3;++pool)
	{
		for(int enemy = 0; enemy < 9; ++enemy)
		{
			for (int i = 0; i < EnochFieldData::GetHeight()/2; ++i)
			{
				for (int j = 0; j < EnochFieldData::GetWidth(); ++j)
				{
					//셀당 30%확률로 등장
					if(rdPersentRange(rdEndgine)<=3)
					{						
						commander.EnemyPoolFieldList[pool][enemy][i][j] =  FreeLancerSaveData(
							static_cast<FreeLancerTemplateID>(rdFLRange(rdEndgine)),1);
					}			
				}
			}			
		}
	}
	return true;
}
 void UCommanderWrapper::GetEnemyPoolFLCount(int PoolNum, vector<pair<FreeLancerTemplateID,int>>& InFLCount)
{
	//FLTid로 소팅할꺼라 map에 보관.
	map<FreeLancerTemplateID,int> FLCountMap;
	for(int enemy = 0; enemy<commander.EnemyPoolFieldList[PoolNum].size();++enemy)
	{
		for(int i = 0; i< commander.EnemyPoolFieldList[PoolNum][enemy].size();++i)
		{
			for(int j = 0; j<commander.EnemyPoolFieldList[PoolNum][enemy][i].size();++j)
			{
				auto FLSaveData = commander.EnemyPoolFieldList[PoolNum][enemy][i][j];
				if(FLSaveData.FLTemplateID==FreeLancerTemplateID::None)
					continue;
				//합성에 사용된 용병 수^(lv-1). 현재는 2^(lv-1).
				int FLOneLevelCount = pow(commander.COMBINE_NUM+1,FLSaveData.FLLevel-1);

				//map에 집어넣고 없으면 바로 넣고, 있으면 숫자를 추가해줌.
				auto CheckCount = FLCountMap.emplace(FLSaveData.FLTemplateID,FLOneLevelCount);
				//이미 존재하면 카운트 증가
				if(!CheckCount.second)
					(CheckCount.first->second)+=FLOneLevelCount;					
			}
		}
	}
	for(auto it = FLCountMap.begin();it!=FLCountMap.end();++it)
	{
		InFLCount.emplace_back(*it);
	}
}

std::vector<FreeLancerSaveData> UCommanderWrapper::RandomRecruitListTemp()
{	
	std::vector<FreeLancerSaveData> Result(commander.MAX_RECRUITSLOT, FreeLancerSaveData());
	std::random_device deviceSeed;
	std::mt19937 rdEndgine(deviceSeed());
	std::uniform_int_distribution<int> rdRange(static_cast<int>(FreeLancerTemplateID::Melee1), static_cast<int>(FreeLancerTemplateID::Ranged1));
	
	for (int i = 0; i < commander.MAX_RECRUITSLOT; ++i)
	{
		Result[i] = FreeLancerSaveData(static_cast<FreeLancerTemplateID>(rdRange(rdEndgine)),1);
	}
	return Result;
}

void UCommanderWrapper::BringFLFieldToInvenWrap(const FVector2D& FieldCell, const int& InvenIdx)
{
	//클릭한 필드 위의 프리랜서 얻기
	AEnochFieldCell* Cell = AEnochField::GetCell(FieldCell);	
	auto FreeLancer = Cell->GetAttachedFreeLancer();
	Vector2D Vec2dFieldCell = Vector2D({static_cast<int>(FieldCell.Y),static_cast<int>(FieldCell.X)});

	//프리랜서 제거(반납). FieldCell로부터의 Detach는 Release함수 내부에서 자동수행
	if (FreeLancer)
	{
		FreeLancer->Release();
		commander.BringFLFieldToInven(Vec2dFieldCell,InvenIdx);
		//슬롯 이미지 업데이트
		CommanderPlayMenuUI->SetInvenSlotMat(InvenIdx);
	}
	
}
bool UCommanderWrapper::DisposeFLInvenToFieldWrap(const FVector2D& FieldCell, const int& InvenIdx)
{
	auto FLTID = commander.InvenFreeLancerList[InvenIdx].FLTemplateID;
	Vector2D Vec2dFieldCell = Vector2D({static_cast<int>(FieldCell.Y),static_cast<int>(FieldCell.X)});

	//데이터 이동 및 스폰 가능 여부 확인
	FailReason DisposeResult = commander.DisposeFLInvenToField(Vec2dFieldCell,InvenIdx);

	//결과 공지. 성공하면 경고문구 지워짐. 아니면 리턴.
	CommanderPlayMenuUI->NoticeFailComment(DisposeResult);
	if(DisposeResult != FailReason::Success)
		return false;				
	
	auto FreeLancer = AEnochActorFactory::SpawnFreeLancer(FLTID);

	if (!FreeLancer)
		return false;	//액터 생성 실패

	//액터를 클릭한 필드에 붙이기
	AEnochFieldCell* Cell = AEnochField::GetCell(FieldCell);	
	Cell->AttachFreeLancer(FreeLancer);

	//데이터 이동.
	commander.DisposeFLInvenToField(Vec2dFieldCell,InvenIdx);
	//이미지 업데이트
	CommanderPlayMenuUI->SetInvenSlotMat(InvenIdx);
	return true;
}
bool UCommanderWrapper::BuyFLRecruitToInvenWrap(const int &RecruitListIdx, const int &InvenListIdx)
{
	FailReason BuyResult = commander.BuyFLRecruitToInven(RecruitListIdx,InvenListIdx);
	
	//결과 공지. 성공하면 경고문구 지워짐. 아니면 리턴.
	CommanderPlayMenuUI->NoticeFailComment(BuyResult);
	if(BuyResult!=FailReason::Success)
		return false;
	//합성 판단
	if(commander.CombinePossibleList[RecruitListIdx].CanCombine)
	{
		BuyAndCombine(RecruitListIdx);
	}
	CommanderPlayMenuUI->SetInvenSlotMat(InvenListIdx);
	CommanderPlayMenuUI->SetRecruitSlotMat(RecruitListIdx);
	//합성 재탐색
	commander.CombineRecruitList();
	//탐색 후 상점 업데이트
	CommanderPlayMenuUI->InitRecruitSlot();
	return true;
}
bool UCommanderWrapper::BuyDisposeFLRecruitToFieldWrap(const FVector2D& FieldCell, const int& RecruitIdx)
{
	auto FLTID = commander.RecruitFreeLancerList[RecruitIdx].FLTemplateID;
	Vector2D Vec2dFieldCell = Vector2D({static_cast<int>(FieldCell.Y),static_cast<int>(FieldCell.X)});

	//데이터 이동 및 구매 성공 여부 확인
	FailReason BuyResult = commander.BuyDisposeFLRecruitToField(Vec2dFieldCell,RecruitIdx);

	//결과 공지. 성공하면 경고문구 지워짐. 아니면 리턴.
	CommanderPlayMenuUI->NoticeFailComment(BuyResult);
	if(BuyResult!=FailReason::Success)
		return false;	

	//합성 판단
	if(commander.CombinePossibleList[RecruitIdx].CanCombine)
	{
		BuyAndCombine(RecruitIdx);
	}
	auto FreeLancer = AEnochActorFactory::SpawnFreeLancer(FLTID);
	if (!FreeLancer)
		return false;	//액터 생성 실패

	//액터를 클릭한 필드에 붙이기
	AEnochFieldCell* Cell = AEnochField::GetCell(FieldCell);	
	Cell->AttachFreeLancer(FreeLancer);

	//이미지 업데이트
	CommanderPlayMenuUI->SetRecruitSlotMat(RecruitIdx);
	//합성 재탐색
	commander.CombineRecruitList();
	//탐색 후 상점 업데이트
	CommanderPlayMenuUI->InitRecruitSlot();
	return true;
	
}
void UCommanderWrapper::BuyAndCombine(const int& RecruitIdx)
{
	auto CombineStruct = commander.CombinePossibleList[RecruitIdx];
	//합성 판단	
	if(!CombineStruct.CanCombine)
	{
		return;		
	}
	//저장된 데이터 삭제
	commander.CombineDelFL(CombineStruct);
	// 인벤 이미지 변경
	for(auto it = CombineStruct.InvenList.begin(); it != CombineStruct.InvenList.end();++it)
	{
		CommanderPlayMenuUI->SetInvenSlotMat(*it);
	}
	//필드 삭제	
	for(auto it = CombineStruct.FieldList.begin(); it != CombineStruct.FieldList.end();++it)
	{		
		FVector2D FieldCell = {static_cast<float>(it->x),static_cast<float>(it->y)};
		//클릭한 필드 위의 프리랜서 얻기
		AEnochFieldCell* Cell = AEnochField::GetCell(FieldCell);	
		auto FreeLancer = Cell->GetAttachedFreeLancer();
		
		//프리랜서 제거(반납). FieldCell로부터의 Detach는 Release함수 내부에서 자동수행
		if (FreeLancer)
		{
			FreeLancer->Release();
		}
	}		
}

void UCommanderWrapper::SellInvenFLWrap(const int &InvenIdx)
{	
	commander.SellInvenFreelancer(InvenIdx);
	//인벤슬롯 이미지 초기화
	CommanderPlayMenuUI->SetInvenSlotMat(InvenIdx);
	//commader 판매 함수 내부에서 상점용병의 합성 가능 여부 재탐색 했음. 상점슬롯 초기화.
	CommanderPlayMenuUI->InitRecruitSlot();
}

void UCommanderWrapper::MoveFLInsideFieldWrap(const FVector2D& SelectCellVec2D, const FVector2D& TargetCellVec2D)
{
	auto SelectFLTID = commander.FieldFreeLancerList[SelectCellVec2D.Y][SelectCellVec2D.X].FLTemplateID;
	auto TargetFLTID = commander.FieldFreeLancerList[TargetCellVec2D.Y][TargetCellVec2D.X].FLTemplateID;
	//선택된게 없으면 그냥 리턴
	if(SelectFLTID == FreeLancerTemplateID::None)
		return;	
	
	AEnochFieldCell* SelectCell = AEnochField::GetCell(SelectCellVec2D);
	AEnochFieldCell* TargetCell = AEnochField::GetCell(TargetCellVec2D);
	
	//프리랜서 제거(반납). FieldCell로부터의 Detach는 Release함수 내부에서 자동수행
	auto FreeLancerSelect = SelectCell->GetAttachedFreeLancer();
	if(FreeLancerSelect)
		FreeLancerSelect->Release();
	
	//목표에 용병 있으면 제거 후, 이전에 선택된 필드셀에서 다시 스폰
	if(TargetFLTID != FreeLancerTemplateID::None)
	{
		auto FreeLancerTarget = TargetCell->GetAttachedFreeLancer();
		if(FreeLancerTarget)
			FreeLancerTarget->Release();
		SelectCell->AttachFreeLancer(AEnochActorFactory::SpawnFreeLancer(TargetFLTID));
	}
	
	TargetCell->AttachFreeLancer(AEnochActorFactory::SpawnFreeLancer(SelectFLTID));
	Vector2D Vec2dSelectCell = Vector2D({static_cast<int>(SelectCellVec2D.Y),static_cast<int>(SelectCellVec2D.X)});
	Vector2D Vec2dTargetCell = Vector2D({static_cast<int>(TargetCellVec2D.Y),static_cast<int>(TargetCellVec2D.X)});

	//내부 정보 교환 스왑완료.
	commander.MoveFLInsideField(Vec2dSelectCell,Vec2dTargetCell);
}


void UCommanderWrapper::SwapInvenSlot(const int &Aidx, const int &Bidx)
{
	commander.SwapInvenSlot(Aidx,Bidx);
	CommanderPlayMenuUI->SetInvenSlotMat(Aidx);
	CommanderPlayMenuUI->SetInvenSlotMat(Bidx);
}

void UCommanderWrapper::SetFieldFreeLancerList(const FVector2D &CellLocation, const FreeLancerSaveData &FreeLancerVar)
{
	commander.FieldFreeLancerList[CellLocation.Y][CellLocation.X] = FreeLancerVar;
}
FreeLancerSaveData UCommanderWrapper::GetFieldFreeLancerList(const int &X, const int &Y)
{
	return commander.FieldFreeLancerList[Y][X];
}
FreeLancerSaveData UCommanderWrapper::GetFieldFreeLancerList(const FVector2D &CellLocation)
{
	return commander.FieldFreeLancerList[CellLocation.Y][CellLocation.X];
}
FreeLancerSaveData UCommanderWrapper::GetRecruitListElement(const int &Idx)
{
	return commander.RecruitFreeLancerList[Idx];
}
FreeLancerSaveData UCommanderWrapper::GetInvenListElement(const int &Idx)
{
	return commander.InvenFreeLancerList[Idx];
}
void UCommanderWrapper::SetRecruitListElement(const int &Idx, const FreeLancerSaveData &val)
{
	commander.RecruitFreeLancerList[Idx] = val;
}
void UCommanderWrapper::SetInvenListElement(const int &Idx, const FreeLancerSaveData &val)
{
	commander.InvenFreeLancerList[Idx] = val;
}