// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <vector>

#include "PlayMenuUIManager.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"
#include "Enoch/Common/Commander.h"

#include "EnemyPoolPanel.generated.h"
using namespace std;
/**
 * 
 */
UCLASS()
class ENOCH_API UEnemyPoolPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	//적 풀 패널 내부 widget 접근용
	/** 내부에 있는 용병들 이미지*/
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* FLImageUniformPanel;
	/** 각 용병 수*/
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* FLNumUniformPanel;
	/** 등급내 총 용병수*/
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* GradeTotalFLNumVerticalBox;
	/**등급별 총 숫자*/
	vector<int> GradeTotalCount;
	/**
	* 상점 슬롯 이미지 저장하는 배열
	* <br>엔진 BP 프로퍼티에서 이미지 교체 가능
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProfileImg")
	TArray<FFreeLancerSlotData> RecruitSlotDataArr;

	UPROPERTY(BlueprintReadWrite)
	int MyPanelNum;
	/** 현재 적풀의 전체 용병 TID와 해당 용병의 수*/
	vector<pair<FreeLancerTemplateID,int>> MyEnemyPoolCountList;

	/** 현재 패널 번호를 바탕으로 적 풀 리스트를 받아서, 풀 화면을 갱신하는 기능*/
	void SetEnemyPoolData(const int& InMyPanelNum);
	
	
};
