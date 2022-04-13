// Fill out your copyright notice in the Description page of Project Settings.


#include "DragImage.h"

#include "EnochGameInstance.h"

void UDragImage::SetDragImg(uint8 FLTID)
{
	UEnochGameInstance* EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());
	auto modelID = FreeLancerTemplate::GetFreeLancerModelID(FLTID);
	DragFreelancerImg -> SetBrushFromMaterial(EGameInstance->MyPlayMenuUI->	 RecruitSlotDataArr[modelID].Material);

}
