// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFight.h"
#include "EnochActorFactory.h"
#include "EnochFreeLancer.h"
#include "EnochProjectile.h"
#include "EnochField.h"
#include "EnochFieldCell.h"
#include "EnochGameModeBase.h"

bool AEnochFight::simulated = false;
// Sets default values
AEnochFight::AEnochFight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnochFight::BeginPlay()
{
	Super::BeginPlay();
	if (simulated) {
		ENLOG(Warning, TEXT("AEnochFight Should Be Destroied Before Recreate it."));
		Destroy();
		return;
	}
	simulated = true;
	//이함수 호출 전에 원래 상대팀이 추가되는 이펙트와 함께, 상대유닛들이 상대진영에 소환되어야함.
	StartFight();
}

void AEnochFight::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Simulator.result == GameResult::None) return;
	Simulator.EndPlay();
	
	for (auto i = 0; i < AEnochActorFactory::GetFreeLancerMax(); i++) {
		auto freeLancer = AEnochActorFactory::GetFreeLancer(i);
		if (freeLancer)
			freeLancer->Reset();
	}

	simulated = false;
}

void AEnochFight::StartFight()
{
	Simulator.BeginPlay();
}

// 제한시간을 두려면, 여기에서 DeltaTime의 합을 계산해서 제한시간이 넘자마자 Tick을 멈추면 될듯
// Called every frame
void AEnochFight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//시뮬레이션 결과 보고 및 결과 화면 송출
	Cast<AEnochGameModeBase>(GetWorld()->GetAuthGameMode())->SetFightResult(Simulator.result);

	//시뮬레이터 결과가 나오면 객체 파괴
	if (Simulator.result != GameResult::UnderSimulated) {
		Destroy();
		return;
	}

	//이번틱에 움직인 모든 결과를 받음
	auto moved = Simulator.Tick(DeltaTime);

	//움직인 프리랜서 view 변환
	for (auto SN_FreeLancer : moved->first) {
		auto freelancer = AEnochActorFactory::GetFreeLancer(SN_FreeLancer);
		if(freelancer != nullptr)
			freelancer->ActInit();
	}
	//움직인 투사체 view 변환
	for (auto SN_Projectile : moved->second) {
		auto projectile = AEnochActorFactory::GetProjectile(SN_Projectile);
		if(projectile != nullptr)
			projectile->ActInit();
	}
	while (!EnochSimulator::logs.empty())
	{
		auto &log = EnochSimulator::logs.front();
		EnochSimulator::logs.pop();
		switch (log.type)
		{
			case SimulateInfoType::MeleeAttack :
				ENLOG(Warning, TEXT("%d번 프리랜서가 %d번 프리랜서를 공격했다! 데미지는 %f였다!"),log.giver, log.taker, log.value);
				break;
		}
	}
}