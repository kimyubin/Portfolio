// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayDefaultPawn.generated.h"

UCLASS()
class ENOCH_API APlayDefaultPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayDefaultPawn();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/** 기본 카메라. 각도 조절용으로 사용안함. 카메라 거리만 조절함.*/
	UPROPERTY(EditAnywhere)
	class UCameraComponent* DefaultPawnCamera;
	/** 기본 카메라의 원점. 이걸 중심으로 회전함.*/
	UPROPERTY(EditAnywhere)
	class USceneComponent* CameraOriginPoint;
	/** 카메라 휠에 의한 회전 속도. 높을 수록 빠름.*/
	UPROPERTY(EditAnywhere)
	float AxisAngleSpeed;
	/** 최대 기울기. 위에서 수직으로 내려보는게 0.*/
	UPROPERTY(EditAnywhere)
	float MaxAngle;
	/** 각도 조절 단계 수.*/
	UPROPERTY(EditAnywhere)
	int TotalNumberOfSteps;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/** 마우스 휠 감지 및 휠 회전 수를 저장. */
	void ZoomInOut(float Axis);

	/**
	 * 카메라 기울기 값을 용병용으로 변환해서 반환. 
	 * 수직으로 바라볼때 용병이 완전히 90도로 누워서 바닥에 파고드는 현상 방지. 90이 아니라 89.9만 뺌.
	 * @return Pitch만 -89.9도
	 */
	static FRotator GetCameraArmRotate(){return FRotator(CameraArmRotate.Pitch-89.9,CameraArmRotate.Yaw,CameraArmRotate.Roll);}

private:
	/** 휠 움직인 횟수 저장. Min:1/Max:20 각도를 20단계로 쪼갬*/ 
	UPROPERTY(VisibleAnywhere)
	int TargetAngleStep;
	/** 한 틱당 단위 각도*/
	UPROPERTY(VisibleAnywhere)
	float UnitAngle;
	UPROPERTY(VisibleAnywhere)
	float MinClamp;
	UPROPERTY(VisibleAnywhere)
	float MaxClamp;
	/** 카메라 앵글 값*/
	static FRotator CameraArmRotate;
};
