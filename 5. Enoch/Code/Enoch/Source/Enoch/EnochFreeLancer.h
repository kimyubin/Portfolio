// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "Common/EnochFreeLancerData.h"
#include "EnochActorFactory.h"
#include "EnochActor.h"
#include "GameFramework/Actor.h"
#include "EnochFreeLancer.generated.h"

/*
 * data를 씬에 표현하는 클래스
 * 외형만을 가지는 클래스로, 일단은 필드에서 사용하기 위해 만들어짐
 */

UCLASS()
class ENOCH_API AEnochFreeLancer : public AEnochActor
{
	GENERATED_BODY()
	
public:	
	
	// Sets default values for this actor's properties
	AEnochFreeLancer();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override; 
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//비 전투 중, 필드 위에서의 좌표 리턴
	UFUNCTION(BlueprintCallable, Category="FreeLancer")
	FVector2D GetFieldLocation() { return FVector2D(data->location.x, data->location.y); }

	//내부데이터에 접근
	EnochFreeLancerData* GetData() { return data; }

	UFUNCTION(BlueprintCallable, Category = "FreeLancer")
	FreeLancerState GetState() { return data != nullptr ? data->GetState() : (FreeLancerState)0; }

	virtual void ActInit() override;

	UFUNCTION(BlueprintCallable, Category = "FreeLancer")
	void Release(); //사용이 끝난 프리랜서에 대해 반드시 호출되어야 하는 함수
	void Reset();	//전투 종료 후 호출되는 함수

	
	int GetHP(){return data->hpNow;}
	int GetMaxHP(){return data->hpMax;}
	int GetMP(){return data->mpNow;}
	int GetMaxMP(){return data->mpMax;}
	
private:

	//전투중 Move동작시 출발점, 도착점
	FVector2D GetMoveSrcLocationOnFight() { return FVector2D(data->moveSrcLocation.x, data->moveSrcLocation.y ); }
	FVector2D GetMoveDstLocationOnFight() { return FVector2D((int32)data->moveDstLocation.x, (int32)data->moveDstLocation.y); }

	//현재 수행하고있는 동작이 몇초째 진행중인가?
	float actTime = 0;
	//Move동작시 Tick에서 호출
	void MoveTick(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent *MeshComponent;
	
	UPROPERTY(VisibleAnywhere)
	class UEnochPaperFlipbookComponent *Flipbook;	
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* HPBarWidgetComp;
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* DeathWidgetComp;
	
	EnochFreeLancerData *data;
	//직업 설정함수
	void SetType(FreeLancerTemplateID type);
	friend class AEnochActorFactory;
};
