﻿// Fill out your copyright notice in the Description page of Project Settings.

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
	UFUNCTION(BlueprintCallable, Category = "FreeLancer")
		FVector2D GetFieldLocation();


	UFUNCTION(BlueprintCallable, Category = "FreeLancer")
		FreeLancerState GetState();

	virtual void ActInit() override;

	UFUNCTION(BlueprintCallable, Category = "FreeLancer")
	void Release(bool update = true); //사용이 끝난 프리랜서에 대해 반드시 호출되어야 하는 함수
	void Reset();	//전투 종료 후 호출되는 함수

	
	int GetHP() { auto data = GetData(); return data == nullptr ? -1 : data->hpNow; }
	int GetMaxHP(){ auto data = GetData(); return data == nullptr ? -1 : data->hpMax;}
	int GetMP(){ auto data = GetData(); return data == nullptr ? -1 : data->mpNow;}
	int GetMaxMP(){ auto data = GetData(); return data == nullptr ? -1 : data->mpMax;}
	void SetActive(bool active) override;
	/** 마우스 오버일 때만 드래그 감지 위젯 활성화. 앞에 있는 용병에서 이미지 날아오는 거 방지
	 */
	void SetDragDetectOnOff(bool OnOff);
	/** 드래그 할 때 스폰된 용병 이미지 안보이게함.
	 * 하위 이미지, 위젯을 모두 끄면 복잡하니까 그냥 카메라 밖으로 옮김.*/
	void SetImageVisualOnOff(bool OnOff);
	FLBattleData* GetData();
private:

	
	//현재 수행하고있는 동작이 몇초째 진행중인가?
	float actTime = 0;
	//Move동작시 Tick에서 호출
	void MoveTick(float DeltaTime);
	void JumpTick(float DeltaTime);

	//내부데이터에 접근
	int SerialNumber;
	int id;


private:
	/** 뷰포트를 향해 액터 이미지만
	 *	전체를 기울이기 위한 애니메이션/플립북/UI 모음용 컴포넌트*/
	UPROPERTY(VisibleAnywhere)
	USceneComponent* AnimGroupComponent;
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent *MeshComponent;
	/**마우스 오버, 클릭 감지용 캡슐. 콜리젼 프로필 올 블락;*/
	UPROPERTY(VisibleAnywhere)
	class UCapsuleComponent* CapsuleComponent;
	UPROPERTY(VisibleAnywhere)
	class UEnochPaperFlipbookComponent *Flipbook;	
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* DeathWidgetComp;
	/** 용병 이름 및 HP 컴포넌트*/
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* UnifiedUpperUIComp;
	/** 용병 드래그 감지용 위젯*/
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* DragDetectWidgetComp;

	
	//직업 설정함수
	void InitFL(int SN);
	/** 용병이 카메라를 바라봄*/
	void RotatorSetforCamera();
	friend class AEnochActorFactory;
};
