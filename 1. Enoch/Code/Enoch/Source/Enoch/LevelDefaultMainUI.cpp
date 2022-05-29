// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "LevelDefaultMainUI.h"

#include "PulicSlotImgArr.h"
#include "Common/EnochFreeLancerData.h"
#include "Kismet/GameplayStatics.h"

void ULevelDefaultMainUI::NativeConstruct()
{
	Super::NativeConstruct();
	
	//공용 슬롯 이미지를 갖고 있는 액트 스폰.
	FName Path = TEXT("Class '/Game/UI/PulicSlotImgArrBP.PulicSlotImgArrBP_C'");
	UClass* TargetBP = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *Path.ToString()));
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),TargetBP,OutActors);

	//이미 스폰된 게 있으면 추가 스폰 안함.
	if(OutActors.Num()==0)
		PublicSlotImgArrActor = GetWorld()->SpawnActor<APulicSlotImgArr>(TargetBP, FTransform::Identity);
	else
		PublicSlotImgArrActor = Cast<APulicSlotImgArr>(OutActors.Last());
}
UMaterialInterface* ULevelDefaultMainUI::GetSlotImg(int FreeLancerTID)
{
	auto modelID = FreeLancerTemplate::GetFreeLancerModelID(FreeLancerTID);
	return PublicSlotImgArrActor->SlotImgArr[modelID];
}
