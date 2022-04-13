// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFight.h"
#include "EnochActorFactory.h"
#include "EnochFreeLancer.h"
#include "EnochProjectile.h"
#include "EnochField.h"
#include "EnochFieldCell.h"
#include "EnochGameModeBasePlay.h"

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
	delay = 0;
	speed = 100;
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
	
	AEnochActorFactory::resetAll();

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
	delay += speed;
	Super::Tick(DeltaTime);
	//시뮬레이션 결과 보고 및 결과 화면 송출
	Cast<AEnochGameModeBasePlay>(GetWorld()->GetAuthGameMode())->SetFightResult(Simulator.result);

	//시뮬레이터 결과가 나오면 객체 파괴
	if (Simulator.result != GameResult::UnderSimulated) {
		Destroy();
		return;
	}

	//이번틱에 움직인 모든 결과를 받음
	while(delay >= 100) {
		delay -= 100;
		Simulator.Tick(DeltaTime);
		AEnochFreeLancer* freelancer;
		while (!EnochSimulator::logs.empty())
		{
			auto& log = EnochSimulator::logs.front();
			switch (log.type)
			{//추후 제대로된 로그 클래스를 구현하고, 다이나믹 캐스트로 읽어내도록 변경해야할듯..
			case SimulateInfoType::MeleeAttack:
				ENLOG(Warning, TEXT("%d번 프리랜서가 %d번 프리랜서를 공격했다! 데미지는 %d였다!"), log.val1, log.val2, log.val3);
				break;
			case SimulateInfoType::Debug:
				ENLOG(Warning, TEXT("Debug Log, %d, %d, %d"), log.val1, log.val2, log.val3);
				break;
			case SimulateInfoType::ChangeState:
				freelancer = AEnochActorFactory::GetFreeLancer(log.val1);
				ENLOG(Warning, TEXT("%d번 프리랜서가 상태를 %d번으로 변경했다! %f"), log.val1, log.val2, log.val3);
				if (freelancer != nullptr)
					freelancer->ActInit();
				else
					ENLOG(Warning, TEXT("근데 왜 널?"));
				break;
			case SimulateInfoType::Move:
				freelancer = AEnochActorFactory::GetFreeLancer(log.val1);
				freelancer->SetActorLocation(AEnochField::GetCell(FVector2D(log.val2, log.val3))->GetActorLocation());
				ENLOG(Warning, TEXT("%d번 프리랜서가 %d,%d로 이동했다!"), log.val1, log.val2, log.val3);
				break;
			case SimulateInfoType::RangeAttack: {
				AEnochActorFactory::GetProjectile(log.val1);
				ENLOG(Warning, TEXT("%d번 프리랜서가 %d번 프리랜서에게 원거리 공격을 시도했다! 생성된 투사체 %d"), log.val1, log.val2, log.val3);
				auto pj = AEnochActorFactory::SpawnProjectile(log.val3);
				pj->ActInit();
			}
											  break;
			case SimulateInfoType::RemovePJ: {
				auto projectile = AEnochActorFactory::GetProjectile(log.val1);
				ENLOG(Warning, TEXT("%d번 투사체가 제거된다!"), log.val1);
				AEnochActorFactory::Release(projectile);
			}
										   break;
			}
			EnochSimulator::logs.pop();
		}
	}
}