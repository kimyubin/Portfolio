// Fill out your copyright notice in the Description page of Project Settings.


#include "DragImage.h"

#include "EnochGameInstance.h"

void UDragImage::SetDragImg(FreeLancerTemplateID FLTID)
{
	UEnochGameInstance* EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());
	DragFreelancerImg -> SetBrushFromMaterial(EGameInstance->MyPlayMenuUI->	 RecruitSlotDataArr[static_cast<int>(FLTID)].Material);

}
