// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyPoolDetailsPanel.h"

#include "Enoch/EnochGameInstance.h"
#include "EnemyPoolDetailSlot.h"
#include "SelectEnemyPoolUI.h"

void UEnemyPoolDetailsPanel::NativeConstruct()
{
	Super::NativeConstruct();
	tempPoolNum = -1;
	tempGradeNum = -1;
}
void UEnemyPoolDetailsPanel::SetSelectEnemyPoolUI(USelectEnemyPoolUI* InSelectUIPtr)
{
	MyParentSelectEnemyPoolUI = InSelectUIPtr;
}

void UEnemyPoolDetailsPanel::UpdateDetailData(const int& PoolNum, int GradeNum)
{
	UEnochGameInstance* EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());
	
	//상세 내용 보여줄 풀의 등급 데이터 가져옴
	auto EnemyNumList =
		EGameInstance->commander->GetEnemyListbyGrade(PoolNum,GradeNum);
	
	//초기화
	ChildrenSlots.Empty();
	EPScrollUniGridPanel->ClearChildren();
	
	//선택된 풀 번호/등급 갱신
	Cast<UTextBlock>(PoolNumText)->SetText(FText::AsNumber(PoolNum));
	Cast<UTextBlock>(GradeNumText)->SetText(FText::AsNumber(GradeNum));	
	tempPoolNum = PoolNum;
	tempGradeNum = GradeNum;
	
	//슬롯 위젯 생성
	for(int i = 0;i<EnemyNumList.size();++i)
	{
		UUserWidget* DetailSlot = CreateWidget<UUserWidget>(this, DetailSlotClass);
		
		Cast<UEnemyPoolDetailSlot>(DetailSlot)->SetDetailSlot(MyParentSelectEnemyPoolUI->GetSlotImg(EnemyNumList[i].first),EnemyNumList[i].second);
		ChildrenSlots.Add(DetailSlot);
		EPScrollUniGridPanel->AddChildToUniformGrid(DetailSlot,i/4,i%4);
	}

}
void UEnemyPoolDetailsPanel::SelectEnemyPool()
{
	UEnochGameInstance* EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());
	EnemyPoolSelectionState EPSS = EnemyPoolSelectionState();
	EPSS.SelectedPool = tempPoolNum;
	EGameInstance->commander->SetEnemyPoolSelectInfo(EPSS);
}
