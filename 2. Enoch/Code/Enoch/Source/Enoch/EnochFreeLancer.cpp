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
#include "PlayDefaultPawn.h"
#include "Components/WidgetComponent.h"
#include "Math/Quat.h"
#include "UIs/FreeLancerDeathUI.h"
#include "Common/FLActs/FLMove.h"
#include "Components/CapsuleComponent.h"
#include "UIs/FieldDragDetector.h"
#include "UIs/FreeLancerUnifiedUpperUI.h"


// Sets default values
AEnochFreeLancer::AEnochFreeLancer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//LineTrace 감지용 태그
	Tags.Add(FName(TEXT("EnochFreeLancer")));

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RENDER"));
	
	AnimGroupComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AnimGroup"));
	AnimGroupComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
	CapsuleComponent->AttachToComponent(AnimGroupComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CapsuleComponent->SetRelativeLocation({0.f,0.f,55.f});
    CapsuleComponent->SetRelativeScale3D({KINDA_SMALL_NUMBER, 1.25f, 1.f});
	CapsuleComponent->SetGenerateOverlapEvents(false);
	CapsuleComponent->SetCollisionProfileName(TEXT("LineTraceOnly"));
	

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	MeshComponent->AttachToComponent(AnimGroupComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponent->SetRelativeScale3D({ 0.5,0.5,0.5 });
	
	Flipbook = CreateDefaultSubobject<UEnochPaperFlipbookComponent>(TEXT("FLIPBOOK"));
	Flipbook->AttachToComponent(AnimGroupComponent,FAttachmentTransformRules::KeepRelativeTransform);

	//UI컴포넌트 생성 및 부착
	UnifiedUpperUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("UnifiedUI"));
	DeathWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DeathText"));
	DragDetectWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DragDetector"));

	//UI컴포넌트에 HP바 설정
	static ConstructorHelpers::FClassFinder<UUserWidget> UnifiedUpperUI(TEXT("/Game/UI/FreeLancerUnifiedUpperUIBP"));
	static ConstructorHelpers::FClassFinder<UUserWidget> DeathUI(TEXT("/Game/UI/FreeLancerDeathText"));
	static ConstructorHelpers::FClassFinder<UUserWidget> DragDetectUI(TEXT("/Game/UI/FieldDragDetector"));

	
	if (UnifiedUpperUI.Succeeded())
	{
		UnifiedUpperUIComp->SetWidgetClass(UnifiedUpperUI.Class);
		UnifiedUpperUIComp->SetDrawSize(FVector2D(100,50));
		UnifiedUpperUIComp->AttachToComponent(AnimGroupComponent,FAttachmentTransformRules::KeepRelativeTransform);
		UnifiedUpperUIComp->SetRelativeLocation(FVector(0.,0.,125.f));
		UnifiedUpperUIComp->SetWidgetSpace(EWidgetSpace::Screen);
	}			
	if(DeathUI.Succeeded())
	{
		DeathWidgetComp->SetWidgetClass(DeathUI.Class);
		DeathWidgetComp->SetDrawSize(FVector2D(65,25));
		DeathWidgetComp->AttachToComponent(AnimGroupComponent,FAttachmentTransformRules::KeepRelativeTransform);
		DeathWidgetComp->SetRelativeLocation(FVector(0.,0.,180.f));
		DeathWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	}
	//주의. 이 컴포넌트는 반드시 마지막에 Attach되어야 함. 다른 UI에 가려져선 안됨.
	if(DragDetectUI.Succeeded())
	{
		DragDetectWidgetComp->SetWidgetClass(DragDetectUI.Class);
		DragDetectWidgetComp->SetDrawSize(FVector2D(150,180));
		DragDetectWidgetComp->AttachToComponent(AnimGroupComponent,FAttachmentTransformRules::KeepRelativeTransform);
		DragDetectWidgetComp->SetRelativeLocation(FVector(0.,0.,20.f));
		DragDetectWidgetComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
		DragDetectWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	}
}

// Called when the game starts or when spawned
void AEnochFreeLancer::BeginPlay()
{
	Super::BeginPlay();
	//Flipbook->SetActionFlipbook(FreeLancerTemplateID::None);
	//
	RotatorSetforCamera();
}

void AEnochFreeLancer::InitFL(int SN)
{
	// 이 시점에서 이미 데이터가 초기화 되었으므로 다시 초기화(InitStatus 등) 안한다...
	auto data = GetData();
	auto TID = data->GetTID();
	if (TID == FreeLancerTemplateID::None) return;
	if (data == nullptr) return;
	Flipbook->SetActionFlipbook(TID);
	Flipbook->Idle();

	// 이름 및 HP바 설정
	//사망 애니메이션 접근
	//드래그 감지용 위젯 설정
	UFreeLancerUnifiedUpperUI* UnifiedUpperUI = Cast<UFreeLancerUnifiedUpperUI>(UnifiedUpperUIComp->GetUserWidgetObject());
	UFreeLancerDeathUI* DeathAnim =Cast<UFreeLancerDeathUI>(DeathWidgetComp->GetUserWidgetObject());
	UFieldDragDetector* DragDetectWidget = Cast<UFieldDragDetector>(DragDetectWidgetComp->GetWidget());

	if(UnifiedUpperUI!=nullptr)
		UnifiedUpperUI->SetOwnerFreeLancer(this);	
	if(DeathAnim!=nullptr)
		DeathAnim->SetOwnerFreeLancer(this);
	if(DragDetectWidget!=nullptr)
		DragDetectWidget->SetOwnerFreeLancer(this);
	SetDragDetectOnOff(false);

	//3D 메쉬를 우선사용하고, 메쉬파일이 없는 경우 2D 플립북 사용
	auto mesh = AFLAnimLoader::GetMesh(TID);
	if(mesh) {
		MeshComponent->SetSkeletalMesh(mesh);
		MeshComponent->SetAnimInstanceClass(*AFLAnimLoader::GetAnim(TID));
		MeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}
	Flipbook->SetVisibility(mesh == nullptr);
}
FVector2D AEnochFreeLancer::GetFieldLocation() 
{ 
	auto data = GetData();
	if (data == nullptr) return FVector2D(0, 0);
	else return FVector2D(data->location.x, data->location.y); 
}

FreeLancerState AEnochFreeLancer::GetState() 
{ 
	auto data = GetData();
	return data != nullptr ? data->GetState() : (FreeLancerState)0; 
}
//return valid data or nullptr
FLBattleData *AEnochFreeLancer::GetData()
{
	return AEnochActorFactory::GetFreeLancerData(SerialNumber);
}

void AEnochFreeLancer::Release(bool update)
{
	if (Factory)
		Factory->Release(this, update);
	
}

void AEnochFreeLancer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//Common 클래스는 가비지컬렉션이 작동하지않으므로, 스스로 메모리를 정리할 수 있도록 유도
	auto data = GetData();
	if (data == nullptr) return;
	data->EndPlay();
}

void AEnochFreeLancer::ActInit()
{
	auto data = GetData();
	if (data == nullptr)
		return;
	auto AnimInstance = Cast<UFLAnimInstance>(MeshComponent->GetAnimInstance());
	auto a = data->direction;
	if (data->GetState() == FreeLancerState::PostMove ||
		data->GetState() == FreeLancerState::PostJump ||
		data->GetState() == FreeLancerState::PostAttack)
		;
	else if(AnimInstance != nullptr) { //deprecated
		
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
	case FreeLancerState::PreMove:
		Flipbook->Idle();
		break;
	case FreeLancerState::PostMove :
		Flipbook->Idle();
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
	actTime = 0;
}

void AEnochFreeLancer::Reset()
{
	auto data = GetData();
	if (data == nullptr) return;
	data->SetState(FreeLancerState::Idle);
	data->SetTarget(nullptr);

	Flipbook->SetRelativeRotation(UKismetMathLibrary::MakeRotFromX(FVector(0, 1, 0)));
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
	auto data = GetData();
	if (data == nullptr) return;
	Super::Tick(DeltaTime);
	
	actTime += DeltaTime;
	switch (data->GetState())
	{
	case FreeLancerState::Idle :
		break;
	case FreeLancerState::PreMove:
	case FreeLancerState::PostMove :
		MoveTick(DeltaTime);
		break;
	case FreeLancerState::PreJump:
	case FreeLancerState::PostJump:
		JumpTick(DeltaTime);
		break;
	case FreeLancerState::PostAttack :
		break;
	case FreeLancerState::PreSkill :
		break;
	case FreeLancerState::Dead:
		break;
	default :
		break;
	}

	RotatorSetforCamera();
	
}

void AEnochFreeLancer::MoveTick(float DeltaTime)
{
	auto data = GetData();
	if (data == nullptr) return;
	auto loc = data->GetMove().floatLocation;
	auto zero = AEnochField::GetCell({ 0,0 });
	auto nextZero = AEnochField::GetCell({ 1,1 });
	auto d = nextZero->GetActorLocation() - zero->GetActorLocation();
	//ENLOG(Warning, TEXT("%f %f"), loc.x, loc.y);
	auto convertedLoc = zero->GetActorLocation() + FVector( d.X * loc.x, d.Y * loc.y, 0);
	SetActorLocation(convertedLoc);
}

void AEnochFreeLancer::JumpTick(float DeltaTime)
{
	auto data = GetData();
	if (data == nullptr) return;
	auto loc = data->GetJump().floatLocation;
	auto zero = AEnochField::GetCell({ 0,0 });
	auto nextZero = AEnochField::GetCell({ 1,1 });
	auto d = nextZero->GetActorLocation() - zero->GetActorLocation();
	//ENLOG(Warning, TEXT("%f %f"), loc.x, loc.y);
	//data->GetJump()
	auto convertedLoc = zero->GetActorLocation() + FVector(d.X * loc.x, d.Y * loc.y, 0);
	SetActorLocation(convertedLoc);
}

void AEnochFreeLancer::SetActive(bool active)
{
	AEnochActor::SetActive(active);
	auto data = GetData();
	if (data == nullptr) return;
	data->BeginPlay();
}

void AEnochFreeLancer::RotatorSetforCamera()
{
	//액터 전체 이미지 기울기 결정.
	//기본 pawn에 달린 카메라 기울기로 결정함.
	//카메라 암 y축(pitch) 기울기의 수직 성분만 따옴.
	APlayDefaultPawn* EDefaultPawn = Cast<APlayDefaultPawn>(Cast<AEnochGameModeBase>(GetWorld()->GetAuthGameMode())->DefaultPawnClass.GetDefaultObject());
	if(EDefaultPawn!=nullptr)
	{		
		AnimGroupComponent->SetRelativeRotation(APlayDefaultPawn::GetCameraArmRotate());		
	}
}
void AEnochFreeLancer::SetDragDetectOnOff(bool OnOff)
{
	DragDetectWidgetComp->InitWidget();
	if (DragDetectWidgetComp != nullptr && DragDetectWidgetComp->GetWidget() != nullptr)
	{
		UFieldDragDetector* DragDetectWidget = Cast<UFieldDragDetector>(DragDetectWidgetComp->GetWidget());
		if (DragDetectWidget->GetIsEnabled())
		{
			if (OnOff)
				DragDetectWidget->SetVisibility(ESlateVisibility::Visible);
			else
				DragDetectWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
		
}
void AEnochFreeLancer::SetImageVisualOnOff(bool OnOff)
{
	static FVector DefaultLocation =  AnimGroupComponent->GetRelativeLocation();
	FVector Location = AnimGroupComponent->GetRelativeLocation();
	if(OnOff)
		Location = DefaultLocation;
	else
		Location.Y -=1000;
	AnimGroupComponent->SetRelativeLocation(Location);
}
