// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "Common/EnochFreeLancerData.h"
#include "Common/EnochProjectileData.h"
#include "PaperFlipbookComponent.h"
#include "Templates/SharedPointer.h"
#include "Containers/Map.h"
#include "GameFramework/Actor.h"
#include "FlipbookLoader.generated.h"

UCLASS()
class ENOCH_API AFlipbookLoader : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlipbookLoader();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static void InitFlipbookMap();
	static TSharedPtr<TMap<FString, TSharedPtr<UPaperFlipbook>>> GetFlipbook(FreeLancerTemplateID id);
	static TSharedPtr<TMap<FString, TSharedPtr<UPaperFlipbook>>> GetFlipbook(ProjectileTemplateID id);

private:
	static TMap<FreeLancerTemplateID, TSharedPtr<TMap<FString, TSharedPtr<UPaperFlipbook>>>> FLFlipbookMap;
	static TMap<ProjectileTemplateID, TSharedPtr<TMap<FString, TSharedPtr<UPaperFlipbook>>>> PJFlipbookMap;
	static bool isInitialized;
	static TSharedPtr<UPaperFlipbook> LoadFlipbook(const wchar_t* path);
};
