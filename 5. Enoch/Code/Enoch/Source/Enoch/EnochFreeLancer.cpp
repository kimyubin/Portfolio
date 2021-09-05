// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFreeLancer.h"
#include "Common/EnochFreeLancerdata.h"
#include "EnochGameModeBase.h"
#include "EnochField.h"
#include "EnochFieldCell.h"
#include "EnochPaperFlipbookComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "FLAnimInstance.h"
#include "FLAnimLoader.h"
#include "Components/WidgetComponent.h"
#include "Math/Quat.h"
#include "UIs/FreeLancerDeathUI.h"
#include "UIs/FreeLancerHPBar.h"


// Sets default values
AEnochFreeLancer::AEnochFreeLancer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RENDER"));
	
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponent->SetRelativeScale3D({ 0.5,0.5,0.5 });

	Flipbook = CreateDefaultSubobject<UEnochPaperFlipbookComponent>(TEXT("FLIPBOOK"));
	Flipbook->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);

	//UI컴포넌트 생성 및 부착
	HPBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBar"));
	DeathWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DeathText"));
	//UI컴포넌트에 HP바 설정
	static ConstructorHelpers::FClassFinder<UUserWidget> HPBarUI(TEXT("/Game/UI/FreelacnerHPBar"));
	static ConstructorHelpers::FClassFinder<UUserWidget> DeathUI(TEXT("/Game/UI/FreeLancerDeathText"));
	
	if (HPBarUI.Succeeded())
	{
		HPBarWidgetComp->SetWidgetClass(HPBarUI.Class);
		HPBarWidgetComp->SetDrawSize(FVector2D(100,15));
	}
			
	if(DeathUI.Succeeded())
	{
		DeathWidgetComp->SetWidgetClass(DeathUI.Class);
		DeathWidgetComp->SetDrawSize(FVector2D(65,25));
	}
		
	
	HPBarWidgetComp->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	HPBarWidgetComp->SetRelativeLocation(FVector(0.,0.,100.f));
	HPBarWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	
	DeathWidgetComp->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
	DeathWidgetComp->SetRelativeLocation(FVector(0.,0.,150.f));
	DeathWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

}

// Called when the game starts or when spawned
void AEnochFreeLancer::BeginPlay()
{
	Super::BeginPlay();
	Flipbook->SetActionFlipbook(FreeLancerTemplateID::None);

	//HP/MP데이터 접근용
	UFreeLancerHPBar* HPBar = Cast<UFreeLancerHPBar>(HPBarWidgetComp->GetUserWidgetObject());
	if(HPBar!=nullptr)
		HPBar->SetOwnerFreeLancer(this);		
	
	//사망 애니메이션 접근		
	UFreeLancerDeathUI* DeathAnim =Cast<UFreeLancerDeathUI>(DeathWidgetComp->GetUserWidgetObject());
	if(DeathAnim!=nullptr)
		DeathAnim->SetOwnerFreeLancer(this);
	
		
}

void AEnochFreeLancer::SetType(FreeLancerTemplateID type)
{
	Flipbook->SetActionFlipbook(type);
	Flipbook->Idle();
	data->SetType(type);
	data->InitStatus(type);


	//3D 메쉬를 우선사용하고, 메쉬파일이 없는 경우 2D 플립북 사용
	auto mesh = AFLAnimLoader::GetMesh(type);
	if(mesh) {
		MeshComponent->SetSkeletalMesh(mesh);
		MeshComponent->SetAnimInstanceClass(*AFLAnimLoader::GetAnim(type));
		MeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}
	Flipbook->SetVisibility(mesh == nullptr);
}


void AEnochFreeLancer::Release()
{
	if (Factory)
		Factory->Release(this);
	
}

void AEnochFreeLancer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//Common 클래스는 가비지컬렉션이 작동하지않으므로, 스스로 메모리를 정리할 수 있도록 유도
	data->EndPlay();
}

void AEnochFreeLancer::ActInit()
{
	auto AnimInstance = Cast<UFLAnimInstance>(MeshComponent->GetAnimInstance());
	auto a = data->direction;
	if(AnimInstance != nullptr) {
		//ENCHECK(AnimInstance != nullptr);

		if(data->GetState() == FreeLancerState::PreAttack || data->GetState() == FreeLancerState::PostAttack)
			AnimInstance->SetState(data->GetState(), 1 / data->attackSpeed);
		else
			AnimInstance->SetState(data->GetState());
		MeshComponent->SetRelativeRotation(UKismetMathLibrary::MakeRotFromX(FVector(a.x, -a.y, 0)));
	}
	else {
		if(a.x == 0) a.x = 1;
		Flipbook->SetRelativeRotation(UKismetMathLibrary::MakeRotFromX(FVector(0, a.x, 0)));
	}
	//MeshComponent->SetRelativeRotation(FRotator(0,0,0));
	switch (data->GetState())
	{
	case FreeLancerState::Idle :
		Flipbook->Idle();
		break;
	case FreeLancerState::PostMove :
		actTime = 0;
		Flipbook->Move();
		break;
	case FreeLancerState::PreAttack :
		Flipbook->Attack();
		break;
	case FreeLancerState::Victory:
		Flipbook->Victory();
		break;
	case FreeLancerState::Dead :
		
		Flipbook->Dead();
		break;
		
	}
	
}

void AEnochFreeLancer::Reset()
{
	data->SetState(FreeLancerState::Idle);
	ActInit();
	data->hpNow = data->hpMax;
	data->mpNow = 0;
	auto cell = AEnochField::GetCell( FVector2D((int32)data->location.x,(int32)data->location.y ));
	if (!cell) return;
	cell->AttachFreeLancer(this);

	
}

// Called every frame
void AEnochFreeLancer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	actTime += DeltaTime;
	switch (data->GetState())
	{
	case FreeLancerState::Idle :
		break;
	case FreeLancerState::PreMove:
		break;
	case FreeLancerState::PostMove :
		MoveTick(DeltaTime);
		break;
	case FreeLancerState::PostAttack :
		break;
	case FreeLancerState::Skill :
		break;
	case FreeLancerState::Dead:
		break;
	default :
		break;
	}
}

void AEnochFreeLancer::MoveTick(float DeltaTime)
{
	if (actTime > data->moveDelay.GetPostDelay()) return;
	auto srcVector = AEnochField::GetCell(GetMoveSrcLocationOnFight())->GetActorLocation();
	auto dstVector = AEnochField::GetCell(GetMoveDstLocationOnFight())->GetActorLocation();
	auto vec = srcVector + (dstVector - srcVector) * (actTime / data->moveDelay.GetPostDelay());
	SetActorLocation(
		vec + FVector(0, 0, FMath::Sin(PI * (actTime / data->moveDelay.GetPostDelay())) * 50)
	);
}
