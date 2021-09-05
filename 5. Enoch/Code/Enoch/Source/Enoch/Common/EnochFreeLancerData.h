// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <queue>
#include <memory>
#include "EnochFieldData.h"
#include "EnochFreeLancerState.h"
#include "EnochActDelay.h"
#include <string>
#include <array>

//위에 UENUM()을 붙이고 uint8로 선언하지 않으면 블루프린트에서 사용 불가능..
/** 용병 목록 <br>capacity: 5<br>begin: Melee1<br>end: Assassin1*/
UENUM()
enum class FreeLancerTemplateID : uint8 {
	None = 0, //NULL
    Melee1 = 1,
    Melee2 = 2,
    Ranged1 = 3,
    Ranged2 = 4,
    Assassin1 = 5,
	TypeNum,
};

UENUM()
enum class SynergyID : uint8 {
	NONE = 0, //NULL
	TEMP1 = 1,
	TEMP2 = 2,
    Num,
};

UENUM()
enum class AIType : uint8 {
	Meele = 0,
	Range = 1,
	Assassin = 2, 
    Num,
};

class FreeLancerLevelTemplate
{
public:
	FreeLancerLevelTemplate();
	
	uint16 hp;
	float hpGen;
	
	uint16 mp;
	float mpGen;
	
	uint16 atkDamage;
	uint16 dmgMin;
	uint16 dmgMax;
	uint16 attackSpeed;
	
	uint16 armor;		// physical Damage Defence, Armor
	uint16 magicArmor;	// Magical Damage Defence, Magic resistance
};

class FreeLancerTemplate
{
public:
	enum
	{
		LEVEL_MAX = 5,
    };

	FreeLancerTemplate();
	
	FreeLancerTemplateID ID;
	wstring name;
	uint8 grade;
	std::vector<SynergyID> synergy;
	AIType AiType;
	
	uint16 attackRange;
	bool isMelee() { return attackRange > 1; }
	float attackDelayBefore;
	float attackDelayAfter;
	float shotSpeed;
	int moveSpeed;
	
	std::array<FreeLancerLevelTemplate, LEVEL_MAX> levelData;

	static const FreeLancerTemplate* GetFreeLancerTemplate(FreeLancerTemplateID ID);
	static bool InitFreeLancerTemplate(wstring path);
	static void UninitFreeLancerTemplate();
};

/**
 *  FreeLancer에 대한 정보를 가지고 있는 일반클래스
 * 이 클래스에서 인게임클래스를 수정하지 않음
 */

class ENOCH_API EnochFreeLancerData
{
public:
	EnochFreeLancerData();
	~EnochFreeLancerData();
	void EndPlay();
	
	Vector2D location;
	bool isEnemy;
	int SerialNumber; //프리랜서간 구분용 시리얼넘버

public:
	FreeLancerState GetState() { return state; }
	void SetState(FreeLancerState stateIn);	//수행 동작 변경
	void SetType(FreeLancerTemplateID typeIn); //프리랜서 타입(직업) 변경
	FreeLancerTemplateID GetType(); // 용병 정보 업데이트용
private :
	FreeLancerState state;	//현재 상태 (수행중인 동작)
	FreeLancerTemplateID type;	//프리랜서 타입(직업)

public:
	Vector2D direction;			//바라보고 있는 방향
	Vector2D moveSrcLocation;	//이동 전 위치 (이동 동작 중에만 사용)
	Vector2D moveDstLocation;	//이동 후 위치 (이동 동작 중에만 사용)
	EnochActDelay moveDelay{ 0.5f, 0.2f, 0.8f };	//이동 딜레이. 현재 0.5초의 딜레이중 선딜 20% 후딜 80%
	EnochActDelay attackDelay{ 1.0f, 0.5f, 0.5f };	//공격 딜레이. 현재 0.7초의 딜레이중 선딜 50% 후딜 50%
	float stateDuration;	//현재 수행하고 있는 모션의 점유 예정시간(남은시간이 아닌 전체시간)
	bool CanUseSpell();	//스킬을 쓸 수 있는 상태인가?
	void Release();
	Vector2D locationOnFight;	//현재 위치(전투중)
	EnochFreeLancerData* target;	//현재 타겟(어그로대상)
	queue<shared_ptr<class EnochProjectileData>> ProjectileAvailable;	//잔여 탄환이지만, 바꿀 예정이므로 사용 금지

	//전투 스테이터스 관련
	void InitStatus(FreeLancerTemplateID type);	//스테이터스 초기화
	static void DoAttack(class EnochFreeLancerData* attacker, class EnochFreeLancerData* defender); //실제 타격시점에 호출되는 함수(근접유닛)
	static void DoAttack(class EnochProjectileData* attacker, class EnochFreeLancerData* defender); //실제 타격시점에 호출되는 함수(원거리유닛) -> 추후 위 함수와 통합예정. 사용금지
	static void CalculateDamage(int damage, class EnochFreeLancerData* defender);	//데미지 계산 및 적용 함수

	int hpMax;	//최대체력
	int hpNow;	//현재체력
	int hpRegeneration;	//체력회복속도

	int mpMax;	//최대마나
	int mpNow;	//현재마나
	int mpRegeneration;	//마나회복속도

	int attackDamage;	//공격력
	float attackSpeed;	//공격속도
	int attackRange;	//사정거리
	
	bool isMelee;	//true일경우 근접유닛, false일경우 원거리유닛
	int defense;	//방어력

public: //시뮬레이션 관련
	void BeginSimulate();
};
