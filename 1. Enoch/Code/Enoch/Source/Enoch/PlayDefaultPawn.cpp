// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayDefaultPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
FRotator APlayDefaultPawn::CameraArmRotate = FRotator::ZeroRotator;
// Sets default values
APlayDefaultPawn::APlayDefaultPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	CameraOriginPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CameraOriginPoint"));
	DefaultPawnCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DefaultPawnCamera"));

	CameraOriginPoint->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	DefaultPawnCamera->AttachToComponent(CameraOriginPoint, FAttachmentTransformRules::KeepRelativeTransform);

	//카메라 기본 셋팅. 상속받은 블루프린트에서 수정가능.
	DefaultPawnCamera->SetFieldOfView(32.f);
	DefaultPawnCamera->SetRelativeLocationAndRotation(FVector(0,0,3000),FRotator(270,0,0));
	CameraOriginPoint->SetRelativeLocationAndRotation(FVector(10,230,20),FRotator(40,0,0));
	AxisAngleSpeed = 425.f;
	TotalNumberOfSteps = 20;
	MaxAngle = 80.f;

	//수정가능한 기본 셋팅을 기반으로 생성된 값들.
	//MaxAngle을 TotalNumberOfSteps 단계로 쪼갬. 기본값은 중간인 40도= 10단계	
	TargetAngleStep = TotalNumberOfSteps/2;
	UnitAngle = MaxAngle / TotalNumberOfSteps;
	MinClamp = UnitAngle * 0.2f;
	MaxClamp = UnitAngle * 0.625f;
}

// Called when the game starts or when spawned
void APlayDefaultPawn::BeginPlay()
{
	Super::BeginPlay();
	//카메라용 기본 pawn 위치 고정. 없으면 멋대로 위치가 튀는 경우가 있음
	SetActorLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);

	CameraArmRotate = CameraOriginPoint->GetRelativeRotation();
	//블루프린트에서 수정 후 초기화용
	TargetAngleStep = TotalNumberOfSteps/2;
	UnitAngle = MaxAngle / TotalNumberOfSteps;
	MinClamp = UnitAngle * 0.2f;
	MaxClamp = UnitAngle * 0.625f;
}

// Called every frame
void APlayDefaultPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//카메라 각도 조정 로직
	FRotator SpringArmAngle = CameraOriginPoint->GetRelativeRotation();
	//입력된 MaxAngle과 현재 TargetAngleStep을 바탕으로 목표 각도 계산
	const float TargetAngle = UnitAngle * TargetAngleStep;
	if (TargetAngle != SpringArmAngle.Pitch)
	{
		//한칸 각도의 하위 20%, 상위 62.5% 초과 구간은 등속도 운동. 이걸 초과하는 범위는 너무 미세하게 움직임.
		//(20단계/최대 80도)4도 기준 0.8도 이하, 2.5도 이상
		float UnknownVar = FMath::Abs(TargetAngle - SpringArmAngle.Pitch);
		UnknownVar = FMath::Clamp(UnknownVar, MinClamp, MaxClamp);
		//최대 각도를 최대 단계로 나눈각일 때 최대 속도. 단위 각도 기준.
		UnknownVar /= UnitAngle;
		//+1은 사인 그래프를 양수로 만드는 용도
		const float TempCalVar = FMath::Sin((UnknownVar - (HALF_PI))) + 1;

		if (TargetAngle > SpringArmAngle.Pitch)
		{
			SpringArmAngle.Pitch += DeltaTime * TempCalVar * AxisAngleSpeed;
			if (TargetAngle < SpringArmAngle.Pitch)
				SpringArmAngle.Pitch = TargetAngle;
		}
		else if (TargetAngle < SpringArmAngle.Pitch)
		{
			SpringArmAngle.Pitch -= DeltaTime * TempCalVar * AxisAngleSpeed;
			if (TargetAngle > SpringArmAngle.Pitch)
				SpringArmAngle.Pitch = TargetAngle;
		}
		SpringArmAngle.Pitch = FMath::Clamp(SpringArmAngle.Pitch, 0.f, MaxAngle);
		CameraOriginPoint->SetRelativeRotation(SpringArmAngle);
		CameraArmRotate = SpringArmAngle;
	}
}

// Called to bind functionality to input
void APlayDefaultPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MouseWheel"), this, &APlayDefaultPawn::ZoomInOut);
}

void APlayDefaultPawn::ZoomInOut(float Axis)
{
	if(Axis>0)
		TargetAngleStep++;
	else if(Axis<0)
		TargetAngleStep--;	
	TargetAngleStep = FMath::Clamp(TargetAngleStep, 0, TotalNumberOfSteps);
}