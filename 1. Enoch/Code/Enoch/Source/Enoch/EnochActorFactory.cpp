// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochActorFactory.h"
#include "EnochGameModeBase.h"
#include "EnochFreeLancer.h"
#include "EnochProjectile.h"
#include "EnochField.h"
#include "EnochFieldCell.h"
#include "FLAnimLoader.h"
#include "EnochGameInstance.h"

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

FLBattleData* AEnochActorFactory::GetFreeLancerData(int32 SerialNumber) 
{
	if (EnochActorDataFactory::instance == nullptr)
		return nullptr;
	else
		return EnochActorDataFactory::instance->GetFreeLancerData(SerialNumber); 
}

EnochProjectileData* AEnochActorFactory::GetProjectileData(int32 SerialNumber)
{ 
	if (EnochActorDataFactory::instance == nullptr)
		return nullptr;
	else
		return EnochActorDataFactory::instance->GetProjectileData(SerialNumber); 
}

//액터를 오브젝트 풀에서 꺼냄
AEnochFreeLancer* AEnochActorFactory::SpawnFreeLancerByTID(uint8 type, uint8 level, bool isEnemy)
{
	if (!GetFactoryReady()) return nullptr;

	auto SerialNumber = Factory->CommonFactory->SpawnFreeLancer();
	auto newFreeLancer = _SpawnFreeLancerActor(SerialNumber);
	if (newFreeLancer == nullptr) return nullptr;
	newFreeLancer->GetData()->InitStatus(type, level, SerialNumber, isEnemy);
	newFreeLancer->InitFL(SerialNumber);
	return newFreeLancer;
}

AEnochProjectile* AEnochActorFactory::SpawnProjectile(int32 SerialNumber)
{
	if (!GetFactoryReady()) return nullptr;

	auto newFreeLancer = _SpawnProjectileActor(SerialNumber);
	if (newFreeLancer == nullptr) return nullptr;
	return newFreeLancer;
}

AEnochProjectile* AEnochActorFactory::_SpawnProjectileActor(int32 SerialNumber)
{
	if (!GetFactoryReady()) return nullptr;
	auto rawData = Factory->CommonFactory->GetProjectileData(SerialNumber);
	if (rawData == nullptr) return nullptr;

	auto world = Factory->GetWorld();
	if (!world) return nullptr;

	//Spawn Actor on Engine
	auto actor = world->SpawnActor<AEnochProjectile>();
	if (!actor)
	{
		ENLOG(Warning, TEXT("Runtime Spawn Error : Cannot Spawn Projectile"));
		return nullptr;
	}

	actor->SerialNumber = SerialNumber;
	actor->Factory = Factory;
	Factory->ProjectileMap.Add(SerialNumber);
	Factory->ProjectileMap[SerialNumber] = actor;

	actor->InitPJ();
	actor->SetActive(true);
	ENLOG(Warning, TEXT("SetActive!"));
	return actor;
}

AEnochFreeLancer* AEnochActorFactory::_SpawnFreeLancerActor(int32 SerialNumber)
{
	if (!GetFactoryReady()) return nullptr;
	auto rawData = Factory->CommonFactory->GetFreeLancerData(SerialNumber);
	if (rawData == nullptr) {
		ENLOG(Warning, TEXT("why?"));
		return nullptr;
	}
	
	auto world = Factory->GetWorld();
	if (!world) return nullptr;

	//Spawn Actor on Engine
	auto actor = world->SpawnActor<AEnochFreeLancer>();
	if (!actor)
	{
		ENLOG(Warning, TEXT("Runtime Spawn Error : Cannot Spawn FreeLancer"));
		return nullptr;
	}

	actor->SerialNumber = SerialNumber;
	actor->Factory = Factory;
	Factory->FreeLancerMap.Add(SerialNumber);
	Factory->FreeLancerMap[SerialNumber] = actor;

	actor->SetActive(true);

	return actor;
}

//사용이 끝난 프리랜서 제거
void AEnochActorFactory::Release(AEnochFreeLancer* actor, bool update)
{
	if (Factory == nullptr) return;

	//액터가 필드에 붙어있는 경우, 필드에서 분리
	auto Cell = AEnochField::GetCell(actor->GetFieldLocation());
	if (Cell)
		Cell->DetachFreeLancer();
	
	//액터를 제거
	actor->SetActive(false);
	auto data = actor->GetData();
	if (data != nullptr) {
		Factory->FreeLancerMap.Remove(actor->SerialNumber);	//액터 제거
		Factory->CommonFactory->DeleteFreeLancer(actor->SerialNumber);	//데이터 제거
	}
	actor->Destroy();
	// 동맹 업데이트
	if(update)
		AEnochActorFactory::UpdateAlliance();
}

void AEnochActorFactory::Release(AEnochProjectile* actor)
{
	if (Factory == nullptr) return;
	if (actor == nullptr) {
		ENLOG(Warning, TEXT("왜 널???"))
		return;
	}

	//액터를 제거
	actor->SetActive(false);
	auto data = actor->GetData();
	if (data != nullptr) {
		Factory->CommonFactory->DeleteProjectile(actor->SerialNumber);	//데이터 제거
	}
	Factory->ProjectileMap.Remove(actor->SerialNumber);	//액터 제거
	//if(actor->Destroy())
	//	ENLOG(Warning, TEXT("디스트로이!"))
	//else
	//	ENLOG(Warning, TEXT("외안댐"))
}

AEnochFreeLancer* AEnochActorFactory::GetFreeLancer(int32 SerialNumber)
{
	if (Factory == nullptr) return nullptr;
	auto actor = Factory->FreeLancerMap.Find(SerialNumber);

	if (actor == nullptr) {
		if (Factory->CommonFactory->GetFreeLancerData(SerialNumber) == nullptr)
			return nullptr;
		else
			return Factory->_SpawnFreeLancerActor(SerialNumber);
	}
	else
		return *actor;
}

AEnochProjectile* AEnochActorFactory::GetProjectile(int32 SerialNumber)
{
	if (Factory == nullptr) return nullptr;
	auto actor = Factory->ProjectileMap.Find(SerialNumber);
	
	if (actor == nullptr)
		return nullptr;
	else
		return *actor;
}

bool AEnochActorFactory::GetFactoryReady()
{
	return Factory != nullptr &&
			Factory->CommonFactory.IsValid();
}

void AEnochActorFactory::resetAll()
{
	if(Factory == nullptr)
		return;
	for(auto& elem : Factory->FreeLancerMap) {
		if (elem.Value)
			elem.Value->Reset();
	}
}

void AEnochActorFactory::UpdateAlliance()
{
	auto& factory = Factory->CommonFactory;
	factory->UpdateAllianceAll();
	factory->UpdateFLAll();
	// UI 업데이트
	auto gi = Cast<UEnochGameInstance>(Factory->GetGameInstance());
	if(gi && gi->MyPlayMenuUI)
		gi->MyPlayMenuUI->UpdateAllianceText(factory->strAlly, factory->strEnemy);
}