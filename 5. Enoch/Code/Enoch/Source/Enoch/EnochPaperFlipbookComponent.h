// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "Common/EnochFreeLancerData.h"
#include "Common/EnochProjectileData.h"
#include "PaperFlipbookComponent.h"
#include "Templates/SharedPointer.h"
#include "Containers/Map.h"
#include "EnochPaperFlipbookComponent.generated.h"

/**
 * AEnochFreeLancer에서 사용할 플립북 컴포넌트. 출력만 담당하는것이 목적
 */
UCLASS()
class ENOCH_API UEnochPaperFlipbookComponent : public UPaperFlipbookComponent
{
	GENERATED_BODY()

public :
		void SetActionFlipbook(FreeLancerTemplateID);
		void SetPJFlipbook(ProjectileTemplateID);
		void Attack();
		void Idle();
		void Dead();
		void Move();
		void Victory();
		
		UEnochPaperFlipbookComponent(){};
private :
	TSharedPtr<TMap<FString, TSharedPtr<UPaperFlipbook>>> book;
};