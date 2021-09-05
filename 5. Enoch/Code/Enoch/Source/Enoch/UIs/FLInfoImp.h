// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"

#include "../Common/EnochFreeLancerData.h"

#include "FLInfoImp.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API UFLInfoImp : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable)
    void UpdateFLInfo();

	UFUNCTION(BlueprintCallable)
    void SetFLSN(int32 SN);

	UFUNCTION(BlueprintCallable)
    void SetFLID(FreeLancerTemplateID ID);

	UFUNCTION(BlueprintCallable)
    void Reset();
    
	UFUNCTION(BlueprintImplementableEvent)
    void SetSwitchIdx(int idx);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 SelectedSN;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FreeLancerTemplateID SelectedID;
	
	UPROPERTY(meta = (BindWidget))
	UImage* SlotImg;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Name;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Star;
	
	//------------------------------
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString DPS;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString APS;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Range;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString PDef;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MDef;

	//------------------------------
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString DmgBase;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString DmgAdd;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString DmgMinus;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ASBase;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ASAdd;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ASMinus;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MSBase;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MSPercent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Reduce;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ArmBase;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ArmAdd;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ArmMinus;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MRBase;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MRAdd;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString MRMinus;
	
};
