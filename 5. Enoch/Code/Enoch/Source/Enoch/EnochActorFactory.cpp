// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochActorFactory.h"
#include "EnochGameModeBase.h"
#include "EnochFreeLancer.h"
#include "EnochProjectile.h"
#include "EnochField.h"
#include "EnochFieldCell.h"
#include "FLAnimLoader.h"

AEnochActorFactory* AEnochActorFactory::Factory;
bool AEnochActorFactory::FactoryReady = false;
// Sets default values
AEnochActorFactory::AEnochActorFactory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
}

// Called when the game starts or when spawned
void AEnochActorFactory::BeginPlay()
{
	Super::BeginPlay();
	CommonFactory = MakeUnique<EnochActorDataFactory>();
	CommonFactory->BeginPlay();

	auto world = GetWorld();
	if (!world) return;

	Factory = this;
	int32 SerialNumber = 0;
}

void AEnochActorFactory::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Factory = nullptr;
	//이 함수 호출시 모든 Actor들의 data 포인터가 할당이 해제되므로, 그전에 data포인터를 참조하는 모든 로직을 종료해야함
	CommonFactory->EndPlay();
	CommonFactory = nullptr;
}

//액터를 오브젝트 풀에서 꺼냄
AEnochFreeLancer* AEnochActorFactory::SpawnFreeLancer(FreeLancerTemplateID type)
{
	if (!GetFactoryReady()) return nullptr;

	//어떤 액터를 꺼낼지는 Common 소스코드에 의존
	auto SerialNumber = (int32)Factory->CommonFactory->SpawnFreeLancer();
	if (SerialNumber < 0) {
		ENLOG(Warning, TEXT("Runtime Spawn Error : Cannot Spawn FreeLancer"));
		return nullptr;
	}
	Factory->CommonFactory->GetFreeLancerData(SerialNumber)->SetType(type);
	return SpawnFreeLancer(SerialNumber);
}

AEnochProjectile* AEnochActorFactory::SpawnProjectile()
{
	if (!GetFactoryReady()) return nullptr;

	auto SerialNumber = (int32)Factory->CommonFactory->SpawnProjectile();
	if (SerialNumber < 0) {
		ENLOG(Warning, TEXT("Runtime Spawn Error : Cannot Spawn FreeLancer"));
		return nullptr;
	}

	return SpawnProjectile(SerialNumber);
}

AEnochProjectile* AEnochActorFactory::SpawnProjectile(int32 SerialNumber)
{
	if (!GetFactoryReady()) return nullptr;
	if (Factory->CommonFactory->GetProjectileData(SerialNumber) == nullptr) return nullptr;
	
	auto world = Factory->GetWorld();
	if (!world) return nullptr;

	//액터를 생성
	auto actor = world->SpawnActor<AEnochProjectile>();
	if (!actor) //스폰실패에 대한 경우는 아직 생각하지 않았음 
	{
		ENLOG(Warning, TEXT("Runtime Spawn Error : Cannot Spawn Projectile"));
		return nullptr;
	}
	actor->Factory = Factory;
	
	while(!Factory->ProjectileMap.IsValidIndex(SerialNumber))
		Factory->ProjectileMap.Add(nullptr);
	Factory->ProjectileMap[SerialNumber] = actor;
	
	//액터를 실제 데이터와 연결
	actor->data = Factory->CommonFactory->GetProjectileData(SerialNumber);

	//액터를 활성화
	actor->SetActive(true);

	return actor;
}

AEnochFreeLancer* AEnochActorFactory::SpawnFreeLancer(int32 SerialNumber)
{
	if (!GetFactoryReady()) return nullptr;
	if (Factory->CommonFactory->GetFreeLancerData(SerialNumber) == nullptr) return nullptr;
	
	auto world = Factory->GetWorld();
	if (!world) return nullptr;

	//액터를 생성
	auto actor = world->SpawnActor<AEnochFreeLancer>();
	if (!actor) //스폰실패에 대한 경우는 아직 생각하지 않았음 
	{
		ENLOG(Warning, TEXT("Runtime Spawn Error : Cannot Spawn FreeLancer"));
		return nullptr;
	}
	actor->Factory = Factory;
	
	while(!Factory->FreeLancerMap.IsValidIndex(SerialNumber))
		Factory->FreeLancerMap.Add(nullptr);
	Factory->FreeLancerMap[SerialNumber] = actor;
	
	//액터를 실제 데이터와 연결
	actor->data = Factory->CommonFactory->GetFreeLancerData(SerialNumber);

	//액터를 활성화
	actor->SetType(actor->data->GetType());
	actor->SetActive(true);

	return actor;
}

//사용이 끝난 프리랜서 제거
void AEnochActorFactory::Release(AEnochFreeLancer* actor)
{
	if (Factory == nullptr) return;

	//액터가 필드에 붙어있는 경우, 필드에서 분리
	auto Cell = AEnochField::GetCell(actor->GetFieldLocation());
	if (Cell)
		Cell->DetachFreeLancer();
	
	//액터를 제거
	actor->SetActive(false);
	Factory->FreeLancerMap[actor->data->SerialNumber] = nullptr;
	actor->data->Release();
	actor->Destroy();
}

AEnochFreeLancer* AEnochActorFactory::GetFreeLancer(int32 SerialNumber)
{
	if (Factory == nullptr) return nullptr;
	if (!Factory->FreeLancerMap.IsValidIndex(SerialNumber)) 
		return SpawnFreeLancer(SerialNumber);
	return Factory->FreeLancerMap[SerialNumber];
}

AEnochProjectile* AEnochActorFactory::GetProjectile(int32 SerialNumber)
{
	if (Factory == nullptr) return nullptr;
	if (!Factory->ProjectileMap.IsValidIndex(SerialNumber)) 
		return SpawnProjectile(SerialNumber);
	return Factory->ProjectileMap[SerialNumber];
}

bool AEnochActorFactory::GetFactoryReady()
{
	return Factory != nullptr &&
			Factory->CommonFactory.IsValid();
}