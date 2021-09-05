// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enoch/Common/EnochFreeLancerData.h"

#include "FieldDragDetector.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API UFieldDragDetector : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	//드래그 직전에 선택되었던 셀 용병 정보
	FreeLancerTemplateID MyFreeLancerTID;
	//드래그 직전에 선택되었던 셀 좌표
	FVector2D MyCurrentCellLocation;

};
