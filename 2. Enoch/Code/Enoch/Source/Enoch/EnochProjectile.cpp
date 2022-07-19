// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochProjectile.h"
#include "EnochActorFactory.h"
#include "Common/PJActs/ProjectileAct.h"
#include "FlipbookLoader.h"
#include "EnochPaperFlipbookComponent.h"
#include "EnochField.h"
#include "EnochFieldCell.h"

AEnochProjectile::AEnochProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RENDER"));

	Flipbook = CreateDefaultSubobject<UEnochPaperFlipbookComponent>(TEXT("FLIPBOOK"));
	Flipbook->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
}

void AEnochProjectile::BeginPlay()
{
	Super::BeginPlay();
}
void AEnochProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AEnochProjectile::Tick(float DeltaTime)
{
	auto data = GetData();
	if (data == nullptr) return;
	auto loc = data->location;
	auto zero = AEnochField::GetCell({ 0,0 });
	auto nextZero = AEnochField::GetCell({ 1,1 });
	auto d = nextZero->GetActorLocation() - zero->GetActorLocation();
	auto convertedLoc = zero->GetActorLocation() + FVector(d.X * loc.x, d.Y * loc.y, 10);
	SetActorLocation(convertedLoc);
}

void AEnochProjectile::InitPJ()
{
	auto data = GetData();
	Flipbook->SetActionFlipbook(101);
	
}

void AEnochProjectile::ActInit()
{
	auto data = GetData();
}
EnochProjectileData* AEnochProjectile::GetData()
{
	return AEnochActorFactory::GetProjectileData(SerialNumber);
}