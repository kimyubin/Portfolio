// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enoch/EnochFreeLancer.h"

#include "FreeLancerHPBar.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API UFreeLancerHPBar : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	void SetOwnerFreeLancer(AEnochFreeLancer* InFL){OwnerFreeLancer = InFL;}
	TWeakObjectPtr<AEnochFreeLancer> OwnerFreeLancer;

	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HPProgressBar;
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* MPProgressBar;
	
};
