// Fill out your copyright notice in the Description page of Project Settings.


#include "FLInfoImp.h"

#include "Enoch/EnochGameInstance.h"
#include "../Common/EnochFreeLancerData.h"

void UFLInfoImp::NativeConstruct()
{
	SelectedSN = 0;
	SelectedID = FreeLancerTemplateID::None;
	Cast<UEnochGameInstance>(GetGameInstance())->FlInfo = this;
	Reset();
}

void UFLInfoImp::UpdateFLInfo()
{
	if(SelectedSN != 0 && SelectedID != FreeLancerTemplateID::None)
		Reset();
	// 선택 안된 상태
	else if(SelectedSN == 0 && SelectedID == FreeLancerTemplateID::None)
	{
		SetSwitchIdx(0);
	}
	// 선택된 상태
	else
	{
		SetSwitchIdx(1);

		auto tmp = FreeLancerTemplate::GetFreeLancerTemplate(SelectedID);
		if( !tmp )
			return;
		auto &lvData = tmp->levelData[0];
		Name = WCHAR_TO_TCHAR(tmp->name.c_str());

		//이미지 세팅
		auto brush = Cast<UEnochGameInstance>(GetGameInstance())
			->MyPlayMenuUI->RecruitSlotDataArr[static_cast<int>(SelectedID)];
		SlotImg->SetBrushFromMaterial(brush.Material);

		// Star > 일단 그래이드랑 같이 표시
		std::string star = "☆/";
		for( int i = 0; i < tmp->grade; i++ )
			star += "☆";
		Star = star.c_str();
		Range = FString::Printf(TEXT("%d"), tmp->attackRange);

		DmgBase = FString::Printf(TEXT("%d"), lvData.atkDamage);
		ASBase  = FString::Printf(TEXT("%d"), lvData.attackSpeed);
		MSBase  = FString::Printf(TEXT("%d"), tmp->moveSpeed);
		MSPercent = FString::Printf(TEXT("%d%%"), 100);
		Reduce	= FString::Printf(TEXT("%d%%"), 0);
		ArmBase = FString::Printf(TEXT("%d"), lvData.armor);
		MRBase  = FString::Printf(TEXT("%d"), lvData.magicArmor);

		// DPS등은 임시로 계산
		float AtkPerSec = 1.7 / (1.0 + lvData.attackSpeed / 100);
		DPS = FString::Printf(TEXT("%.2f"), AtkPerSec * lvData.atkDamage);
		APS = FString::Printf(TEXT("%.2f"), AtkPerSec);
		float pdef = 0.1 * lvData.armor / (1.0 + 0.1 * (lvData.armor < 0 ? -lvData.armor : lvData.armor));
		PDef = FString::Printf(TEXT("%.2f%%"), pdef);
		MDef = FString::Printf(TEXT("%.2f%%"), ((float)lvData.magicArmor)/100.0);

		// 임시로 추가/감소 데미지 표시
		static int add = 0;
		if (add)
		{
			add = 0;
			DmgAdd = FString::Printf(TEXT("+%d"), 5);
			ASAdd = FString::Printf(TEXT("+%d"), 5);
			ArmAdd = FString::Printf(TEXT("+%d"), 5);
			MRAdd = FString::Printf(TEXT("+%d"), 5);
			DmgMinus.Empty();
			ASMinus.Empty();
			ArmMinus.Empty();
			MRMinus.Empty();
		}
		else
		{
			add = 1;
			DmgAdd.Empty();
			ASAdd.Empty();
			ArmAdd.Empty();
			MRAdd.Empty();
			DmgMinus = FString::Printf(TEXT("-%d"), 5);
			ASMinus = FString::Printf(TEXT("+%d"), 5);
			ArmMinus = FString::Printf(TEXT("+%d"), 5);
			MRMinus = FString::Printf(TEXT("+%d"), 5);
		}
	}
}

void UFLInfoImp::SetFLSN(int32 SN)
{
	SelectedSN = SN;
	SelectedID = FreeLancerTemplateID::None;
	UpdateFLInfo();
}

void UFLInfoImp::SetFLID(FreeLancerTemplateID ID)
{
	SelectedSN = 0;
	SelectedID = ID;
	UpdateFLInfo();
}

void UFLInfoImp::Reset()
{
	SelectedSN = 0;
	SelectedID = FreeLancerTemplateID::None;
	UpdateFLInfo();
}
