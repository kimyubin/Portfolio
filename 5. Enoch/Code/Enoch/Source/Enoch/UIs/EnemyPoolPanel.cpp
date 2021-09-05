// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPoolPanel.h"

#include "Components/TextBlock.h"
#include "Enoch/EnochGameInstance.h"

void UEnemyPoolPanel::SetEnemyPoolData(const int& InMyPanelNum)
{
	MyPanelNum = InMyPanelNum;
	UEnochGameInstance* EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());
	MyEnemyPoolCountList.clear();
	EGameInstance->commander->GetEnemyPoolFLCount(InMyPanelNum,MyEnemyPoolCountList);

	//등급별로 용병 분류. TID, count
	vector<vector<pair<FreeLancerTemplateID,int>>> FLByGrade(5,vector<pair<FreeLancerTemplateID,int>>());
	
	TArray<UWidget*> FLImageUniformChildren = FLImageUniformPanel->GetAllChildren();
	TArray<UWidget*> FLNumUniformChildren = FLNumUniformPanel->GetAllChildren();
	TArray<UWidget*> GradeCountChildren = GradeTotalFLNumVerticalBox->GetAllChildren();
	GradeTotalCount = vector<int>(5,0);
	for(int i = 0;i<MyEnemyPoolCountList.size();++i)
	{
		//등급별로 정리
		uint8 FreeLancerGrade = FreeLancerTemplate::GetFreeLancerTemplate(MyEnemyPoolCountList[i].first)->grade;
		
		FLByGrade[FreeLancerGrade-1].emplace_back(MyEnemyPoolCountList[i]);
		//등급내 총합 정리
		GradeTotalCount[FreeLancerGrade-1]+=MyEnemyPoolCountList[i].second;
	}
	//등급별 총 인원수 수정
	for(int i = 0;i<GradeTotalFLNumVerticalBox->GetChildrenCount();++i)
	{
		Cast<UTextBlock>(GradeCountChildren[i])->SetText(FText::AsNumber(GradeTotalCount[i]));
	}

	//등급 별로 정리한 자료를 바탕으로 이미지 및 숫자 변경
	//uniform패널이라 5단위로 정리
	for(int grd = 0; grd < FLByGrade.size();++grd)
	{		
		for(int i = 0;i<FLByGrade[grd].size();++i)
		{			
			auto Mat = RecruitSlotDataArr[static_cast<int>(FLByGrade[grd][i].first)].Material;
			//등급이 5단위로 나뉘어짐. 유니폼 패널 자녀 개수보다 많아지면 다음패스
			int panelSlotNum = ((grd)*5)+(i);
			if(panelSlotNum > FLImageUniformPanel->GetChildrenCount())
				continue;;
			Cast<UImage>(FLImageUniformChildren[panelSlotNum])->SetBrushFromMaterial(Mat);
			Cast<UTextBlock>(FLNumUniformChildren[panelSlotNum])->SetText(FText::AsNumber(FLByGrade[grd][i].second));
		}
		
	}
	//유니폼 패널 자식 개수 초과시 리턴
	
}
