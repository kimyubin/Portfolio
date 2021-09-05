// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FreeLancerHPBar.h"
#include "Components/ProgressBar.h"


void UFreeLancerHPBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(!OwnerFreeLancer.IsValid())
		return;
	if(OwnerFreeLancer->GetMaxHP()>0)
		HPProgressBar->SetPercent(static_cast<float>(OwnerFreeLancer->GetHP())/static_cast<float>(OwnerFreeLancer->GetMaxHP()));
	if(OwnerFreeLancer->GetMaxMP()>0)
		MPProgressBar->SetPercent(static_cast<float>(OwnerFreeLancer->GetMP())/static_cast<float>(OwnerFreeLancer->GetMaxMP()));
	
}
