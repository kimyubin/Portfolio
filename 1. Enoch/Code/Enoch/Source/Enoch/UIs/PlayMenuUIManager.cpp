// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PlayMenuUIManager.h"

#include "Components/UniformGridPanel.h"
#include "Enoch/UIs/UniformSlot.h"
#include "Enoch/EnochGameInstance.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Components/VerticalBox.h"
#include "Enoch/EnochDragDropOP.h"
#include "Enoch/EnochFieldDropProtector.h"
#include "Enoch/EnochFieldSaveSlot.h"
#include "Enoch/EnochSaveField.h"

using namespace std;

void UPlayMenuUIManager::NativeConstruct()
{
	Super::NativeConstruct();
	//슬롯 이미지 초기화
	InitRecruitSlot();
	InitInvenSlot();
	InitSaveSlot();
	Cast<UEnochGameInstance>(GetGameInstance())->MyPlayMenuUI = this;
	Cast<UEnochGameInstance>(GetGameInstance())->commander->CommanderPlayMenuUI = this;
	FailCommentText->SetText(FText());
}


void UPlayMenuUIManager::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	//DragImage->SetVisibility(ESlateVisibility::Hidden);
}
void UPlayMenuUIManager::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	//DragImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);	
}
void UPlayMenuUIManager::DragImageVisible()
{
	DragImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}
void UPlayMenuUIManager::DragImageHide()
{
	DragImage->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayMenuUIManager::InitSaveSlot()
{
	//save슬롯 번호 부여 등의 초기화
	TArray<UWidget*> SaveChildren = SaveSlotVerticalBox->GetAllChildren();	
	const int SaveSlotCount = SaveSlotVerticalBox->GetChildrenCount();
	
	for (int i = 0; i < SaveSlotCount; i++)
	{
		//슬롯 번호 부여
		Cast<UEnochFieldSaveSlot>(SaveChildren[i])->SlotNum=i;
		// //이름 바꿔주기
		// FString temp = "";
		// temp.Append(FString::FromInt(i));
		Cast<UEnochFieldSaveSlot>(SaveChildren[i])->SaveText->SetText(FText::FromString(FString::FromInt(i)));
	}	
}

void UPlayMenuUIManager::InitRecruitSlot()
{
	TArray<UWidget*> RecruitChildren = RecruitUniformGridPanel->GetAllChildren();	
	const int RecruitSlotCount = RecruitUniformGridPanel->GetChildrenCount();
	
	for (int i = 0; i < RecruitSlotCount; i++)
	{
		//슬롯에 해당 하는 용병 정보.
		auto MyRecruitSlotFreeLancerInfo
			= Cast<UEnochGameInstance>(GetGameInstance())->commander->GetRecruitListElement(i);
		
		//슬롯 자신의 일련번호 부여
		Cast<UUniformSlot>(RecruitChildren[i])->SlotNum = i;
		//슬롯 패널 종류 뷰여
		Cast<UUniformSlot>(RecruitChildren[i])->SetMyParentPanelName(PanelType::RecruitPanel);
		
		//현재 슬롯이 보유한 용병의 FreelanceTID 부여
		Cast<UUniformSlot>(RecruitChildren[i])->SetMyFreeLancerInfo(MyRecruitSlotFreeLancerInfo);
		
		//현재 슬롯이 가진 용병 이미지 가져오기
		auto modelID = FreeLancerTemplate::GetFreeLancerModelID(MyRecruitSlotFreeLancerInfo.FLTemplateID);
		Cast<UUniformSlot>(RecruitChildren[i])->
			SetMat(RecruitSlotDataArr[modelID].Material);
	}
}
void UPlayMenuUIManager::InitInvenSlot()
{
	TArray<UWidget*> InvenChildren = InvenUniformGridPanel->GetAllChildren();	
	const int InvenSlotCount = InvenUniformGridPanel->GetChildrenCount();
	
	for (int i = 0; i < InvenSlotCount; i++)
	{
		//슬롯에 해당 하는 용병 정보.
		auto MyInvenSlotFreeLancerInfo
			= Cast<UEnochGameInstance>(GetGameInstance())->commander->GetInvenListElement(i);
		
		//슬롯 자신의 일련번호 부여
		Cast<UUniformSlot>(InvenChildren[i])->SlotNum = i;
		//슬롯 패널 종류 뷰여
		Cast<UUniformSlot>(InvenChildren[i])->SetMyParentPanelName(PanelType::InvenPanel);
		
		//슬롯이 보유한 용병 FID부여
		Cast<UUniformSlot>(InvenChildren[i])->SetMyFreeLancerInfo(MyInvenSlotFreeLancerInfo);
		
		//현재 슬롯이 가진 용병 이미지 가져오기
		auto modelID = FreeLancerTemplate::GetFreeLancerModelID(MyInvenSlotFreeLancerInfo.FLTemplateID);
		Cast<UUniformSlot>(InvenChildren[i])->
			SetMat(RecruitSlotDataArr[modelID].Material);
	}
}
void UPlayMenuUIManager::SetRecruitSlotMat(const int& ChangeSlot)
{
	auto MyRecruitSlotFreeLancer
		= Cast<UEnochGameInstance>(GetGameInstance())->commander->GetRecruitListElement(ChangeSlot);	
	TArray<UWidget*> RecruitChildren = RecruitUniformGridPanel->GetAllChildren();
	
	//현재 슬롯이 보유한 용병 정보 갱신
	Cast<UUniformSlot>(RecruitChildren[ChangeSlot])->SetMyFreeLancerInfo(MyRecruitSlotFreeLancer);
	//해당 갱신된 용병 이미지 업데이트
	auto modelID = FreeLancerTemplate::GetFreeLancerModelID(MyRecruitSlotFreeLancer.FLTemplateID);
	Cast<UUniformSlot>(RecruitChildren[ChangeSlot])->
		SetMat(RecruitSlotDataArr[modelID].Material);
}
void UPlayMenuUIManager::SetInvenSlotMat(const int& ChangeSlot)
{
	auto MyInvenSlotFreeLancer
		= Cast<UEnochGameInstance>(GetGameInstance())->commander->GetInvenListElement(ChangeSlot);
	TArray<UWidget*> InvenChild = InvenUniformGridPanel->GetAllChildren();

	//현재 슬롯이 보유한 용병 정보 갱신
	Cast<UUniformSlot>(InvenChild[ChangeSlot])->SetMyFreeLancerInfo(MyInvenSlotFreeLancer);
	//해당 갱신된 용병 이미지 업데이트
	auto modelID = FreeLancerTemplate::GetFreeLancerModelID(MyInvenSlotFreeLancer.FLTemplateID);
	Cast<UUniformSlot>(InvenChild[ChangeSlot])->SetMat(RecruitSlotDataArr[modelID].Material);	
}

void UPlayMenuUIManager::RerollButtonRun()
{
	//선택 해제
	Cast<UEnochGameInstance>(GetGameInstance())->DeselectedFLUpdate();
	//새로운 상점 리스트 할당 및 슬롯 이미지 업데이트
	Cast<UEnochGameInstance>(GetGameInstance())->commander->ReqNewRecruitList();
	
	InitRecruitSlot();
}
void UPlayMenuUIManager::SellButtonRun()
{
	auto SelectFL = Cast<UEnochGameInstance>(GetGameInstance())->SelectedFreeLancerInfo;
	if(SelectFL.SelectedPanel!=PanelType::InvenPanel)
		return;
	Cast<UEnochGameInstance>(GetGameInstance())->commander->SellInvenFLWrap(SelectFL.SlotNum);
	//선택해제
	Cast<UEnochGameInstance>(GetGameInstance())->DeselectedFLUpdate();
	
}


void UPlayMenuUIManager::NoticeFailComment(const FailReason& FailComment)
{
	FText CommentText;
	switch (FailComment)
	{
	case(FailReason::None):
		break;
	case(FailReason::Success):
		CommentText = FText::FromString(TEXT(""));
		break;
	case(FailReason::NotEnoughGolds):		
		CommentText = FText::FromString(TEXT("골드가 부족합니다"));		
		break;
	case(FailReason::FieldAlreadyFull):
		CommentText = FText::FromString(TEXT("필드가 꽉 찼습니다"));		
		break;
	default:
		break;
	}
	
	FailCommentText->SetText(CommentText);
	GetGameInstance()->GetTimerManager().SetTimer(TimeHandler,this,&UPlayMenuUIManager::ClearFailComment,1.0f,true);
	
}
void UPlayMenuUIManager::ClearFailComment()
{		
	FailCommentText->SetText(FText());
	GetGameInstance()->GetTimerManager().ClearTimer(TimeHandler);
}
bool UPlayMenuUIManager::IsHoverUI()
{
	if(RecruitFieldDropProtector->IsDragHoverd()|InvenFieldDropProtector->IsDragHoverd())
		return true;
	if(UUniformSlot::IsHoverUniformSlot())
		return true;
	return false;
}

void UPlayMenuUIManager::UpdateAllianceText(wstring ally, wstring enemy)
{
	txtAllianceAlly->SetText(FText::FromString(ally.c_str()));
	txtAllianceEnemy->SetText(FText::FromString(enemy.c_str()));
}
