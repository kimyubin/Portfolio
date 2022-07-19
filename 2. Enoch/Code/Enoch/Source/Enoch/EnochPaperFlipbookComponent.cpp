// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochPaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "FlipbookLoader.h"
#include "EnochFreeLancer.h"
#include "EnochProjectile.h"

void UEnochPaperFlipbookComponent::SetActionFlipbook(uint8 type)
{
	book = AFlipbookLoader::GetFlipbook(type);
	Idle();
	SetRelativeRotation(FRotator(0, 90, 0));
}

//재생동작을 변경하는 함수들
void UEnochPaperFlipbookComponent::Attack()
{
	if (!book) return;
	auto anim = book->Find("Attack");
	if(!anim) return;
	SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));
	//SetRelativeRotation(FRotator(0, 90, 0));
	SetFlipbook(DuplicateObject<UPaperFlipbook>(anim->Get(), GetOuter()));
}
void UEnochPaperFlipbookComponent::Idle()
{
	if (!book) return;
	auto anim = book->Find("Idle");
	if(!anim) return;
	SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));
	//SetRelativeRotation(FRotator(0, 90, 0));
	SetFlipbook(DuplicateObject<UPaperFlipbook>(anim->Get(), GetOuter()));
}
void UEnochPaperFlipbookComponent::Move()
{
	if (!book) return;
	auto anim = book->Find("Move");
	if(!anim) return;
	SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));
	//SetRelativeRotation(FRotator(0, 90, 0));
	SetFlipbook(DuplicateObject<UPaperFlipbook>(anim->Get(), GetOuter()));
}

void UEnochPaperFlipbookComponent::Victory()
{
	if (!book) return;
	auto anim = book->Find("Victory");
	if(!anim) return;
	SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));
	//SetRelativeRotation(FRotator(0, 90, 0));
	SetFlipbook(DuplicateObject<UPaperFlipbook>(anim->Get(), GetOuter()));
}

void UEnochPaperFlipbookComponent::Dead()
{
	if (!book) return;
	auto anim = book->Find("Dead");
	if(!anim) return;
	SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
	SetRelativeRotation(FRotator(0, 90, 90));
	SetFlipbook(DuplicateObject<UPaperFlipbook>(anim->Get(), GetOuter()));
}
