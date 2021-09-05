// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFreeLancerData.h"
#include "EnochFreeLancerState.h"
#include "EnochFieldData.h"
#include "EnochActorDataFactory.h"
#include "EnochFieldCellData.h"
#include "EnochProjectileData.h"
#include "EnochSimulator.h"
#include <map>
#include "CSVParser.h"

#include "../Enoch.h"

#include <memory>
using namespace std;

static map<FreeLancerTemplateID, shared_ptr<FreeLancerTemplate>> mFreeLancerTemplate;

const FreeLancerTemplate* FreeLancerTemplate::GetFreeLancerTemplate(FreeLancerTemplateID ID)
{
	auto it = mFreeLancerTemplate.find(ID); 
	if(it != mFreeLancerTemplate.end())
		return it->second.get();
	return nullptr;
}

// column 열 순서 바뀔 경우 여기를 바꿔줘야함
enum FreeLancerColumn
{
	COLUMN_ID,
	COLUMN_NAME,
	COLUMN_GRADE,
	COLUMN_SYNERGY,
	COLUMN_AI_TYPE,
	COLUMN_ATTACK_RANGE,
	COLUMN_ATTACK_DELAY_BEFORE,
	COLUMN_ATTACK_DELAY_AFTER,
	COLUMN_SHOT_SPEED,
	COLUMN_LEVEL,
	COLUMN_HP,
	COLUMN_HPGEN,
	COLUMN_MP,
	COLUMN_MPGEN,
	COLUMN_ATTACK_SPEED,
	COLUMN_DAMAGE,
	COLUMN_DMG_RANGE,
	COLUMN_ARMOR,
	COLUMN_MAGIC_ARMOR,
};

FreeLancerTemplate::FreeLancerTemplate()
	:ID(FreeLancerTemplateID::None), name(L""),grade(1),AiType(AIType::Meele),
	attackRange(1),attackDelayBefore(0.1),attackDelayAfter(0.1),shotSpeed(1),moveSpeed(100)
{}

FreeLancerLevelTemplate::FreeLancerLevelTemplate()
	:hp(1000),hpGen(0.0f),mp(100),mpGen(0.0f)
	,atkDamage(10),dmgMin(10),dmgMax(10),attackSpeed(100)
	,armor(0),magicArmor(0)
{}

bool FreeLancerTemplate::InitFreeLancerTemplate(wstring path)
{
	const static wstring filename = L"FreeLancerTemplateData.csv";
	wstring path_data = path + L"/" + filename;
	
	CSVParser parser(path_data);
	if(!parser.success)
	{
		UE_LOG(Enoch, Log, TEXT("Success : %s"), parser.success ? TEXT("TRUE") : TEXT("FALSE"));
		return false;	
	}

	// 첫줄은 헤더라 무시
	const auto &content = parser.content;
	const int size_data = ( content.size() - 1 ) / LEVEL_MAX;

	if( !mFreeLancerTemplate.empty() )
		FreeLancerTemplate::UninitFreeLancerTemplate();
	
	for( int i = 0; i < size_data; i++)
	{
		const int row_first = 1 + i * LEVEL_MAX;
		shared_ptr<FreeLancerTemplate> elem(new FreeLancerTemplate());
		auto &first_content = content[row_first];
		
		elem->ID			= static_cast<FreeLancerTemplateID>(stoi(first_content[COLUMN_ID]));
		elem->name			= first_content[COLUMN_NAME];
		elem->grade			= stoi(first_content[COLUMN_GRADE]);

		for(int j = 0; j < LEVEL_MAX; j++)
		{
			auto &row_tmp = content[row_first+j];
			if(row_tmp.size() < i || row_tmp[COLUMN_GRADE].empty())
				break;
			SynergyID synergyID = static_cast<SynergyID>(stoi(row_tmp[COLUMN_GRADE]));
			elem->synergy.push_back(synergyID);
		}
		
		elem->AiType		= static_cast<AIType>(stoi(first_content[COLUMN_AI_TYPE]));
		elem->attackRange	= stoi(first_content[COLUMN_ATTACK_RANGE]);
		elem->attackDelayBefore	= stof(first_content[COLUMN_ATTACK_DELAY_BEFORE]);
		elem->attackDelayAfter	= stof(first_content[COLUMN_ATTACK_DELAY_AFTER]);
		if (!first_content[COLUMN_SHOT_SPEED].empty())
			elem->shotSpeed		= stof(first_content[COLUMN_SHOT_SPEED]);
		else
			elem->shotSpeed		= 0;
		
		for(int j = 0; j < LEVEL_MAX; j++)
		{
			auto &row_tmp = content[row_first+j];
			auto &elem_level = elem->levelData[j];
			elem_level.hp			= stoi(row_tmp[COLUMN_HP]);
			elem_level.hpGen		= stof(row_tmp[COLUMN_HPGEN]);
			elem_level.mp			= stoi(row_tmp[COLUMN_MP]);
			elem_level.mpGen		= stof(row_tmp[COLUMN_MPGEN]);
			
			elem_level.attackSpeed	= stoi(row_tmp[COLUMN_ATTACK_SPEED]);
			elem_level.atkDamage	= stoi(row_tmp[COLUMN_DAMAGE]);
			if( !row_tmp[COLUMN_DMG_RANGE].empty() )
			{
				int dmgRange		= stoi(row_tmp[COLUMN_DMG_RANGE]);
				elem_level.dmgMax	= elem_level.atkDamage + dmgRange;
				elem_level.dmgMin	= max( elem_level.atkDamage - dmgRange, 0 );
			}
			else
			{
				elem_level.dmgMax	= elem_level.atkDamage;
				elem_level.dmgMin	= elem_level.atkDamage;
			}
				
			elem_level.armor		= stoi(row_tmp[COLUMN_ARMOR]);
			elem_level.magicArmor	= stoi(row_tmp[COLUMN_MAGIC_ARMOR]);
		}
		
		mFreeLancerTemplate.insert(make_pair(elem->ID, elem));
	}
	
	
	return true;
}

void FreeLancerTemplate::UninitFreeLancerTemplate()
{
	mFreeLancerTemplate.clear();
	return;
}

EnochFreeLancerData::EnochFreeLancerData()
{

}

EnochFreeLancerData::~EnochFreeLancerData()
{
}

void EnochFreeLancerData::Release()
{
	if (EnochActorDataFactory::instance)
		EnochActorDataFactory::instance->Release(this);
}

void EnochFreeLancerData::EndPlay()
{
	;
}

void EnochFreeLancerData::BeginSimulate()
{
	locationOnFight = location;
	target = nullptr;
	if (location.y >= 5) isEnemy = true;
	else isEnemy = false;
	if(isEnemy) direction = {-1,0};
	else direction = {1,0};
}

void EnochFreeLancerData::SetType(FreeLancerTemplateID typeIn)
{
	type = typeIn;
	state = FreeLancerState::Idle;
}
FreeLancerTemplateID EnochFreeLancerData::GetType()
{
	return type;
}


void EnochFreeLancerData::SetState(FreeLancerState stateIn) 
{
	state = stateIn;
	switch (state)
	{
	case FreeLancerState::PreMove:
		stateDuration = moveDelay.GetPreDelay();
		break;
	case FreeLancerState::PostMove :
		stateDuration = moveDelay.GetPostDelay();
		break;
	case FreeLancerState::PreAttack:
		stateDuration = attackDelay.GetPreDelay();
		break;
	case FreeLancerState::PostAttack :
		stateDuration = attackDelay.GetPostDelay();
		break;
	case FreeLancerState::Idle :
		stateDuration = 0.5f;
		target = nullptr;
	}
}

bool EnochFreeLancerData::CanUseSpell()
{
	return false;
}

void EnochFreeLancerData::DoAttack(EnochFreeLancerData* attacker, EnochFreeLancerData* defender)
{
	if (!attacker || !defender)
		return;
	//데미지 계산
	auto damage = (float)(attacker->attackDamage - defender->defense);
	EnochSimulator::logs.push({ SimulateInfoType::MeleeAttack, attacker->SerialNumber, defender->SerialNumber, damage });
	CalculateDamage(damage, defender);
}

void EnochFreeLancerData::DoAttack(EnochProjectileData* attacker, EnochFreeLancerData* defender)
{
	if (!attacker || !defender)
		return;
	//데미지 계산
	auto damage = (int)(attacker->attackDamage - defender->defense);
	CalculateDamage(damage, defender);
	EnochActorDataFactory::instance->Release(attacker);
}

void EnochFreeLancerData::CalculateDamage(int damage, class EnochFreeLancerData* defender)
{
	if (damage >= defender->hpNow) {
		defender->hpNow = 0;
		//이후 체력 0에서 안죽는 스킬이 추가된다면 여기에 조건문 추가해야할듯

		//전투 중 사망
		defender->state = FreeLancerState::Dead;
		defender->target = nullptr;
		EnochFieldData::GetData(defender->locationOnFight)->SetFreeLancerOnFight(nullptr);
	}
	else
		defender->hpNow -= damage;
}


//기본스탯 템플릿 사용
void EnochFreeLancerData::InitStatus(FreeLancerTemplateID typeIn)
{
	//일단 하드코딩.
	switch (typeIn)
	{
	case FreeLancerTemplateID::Melee1 :
		hpMax = 800;
		hpRegeneration = 1;
		mpMax = 100;
		mpRegeneration = 0;
		defense = 1;
		attackRange = 1;
		attackDamage = 150;
		attackSpeed = 1;
		attackDelay.SetPreDelayRatio(0.5f);
		attackDelay.SetPostDelayRatio(0.5f);
		isMelee = true;
		break;
	case FreeLancerTemplateID::Ranged1 :
		hpMax = 600;
		hpRegeneration = 1;
		mpMax = 100;
		mpRegeneration = 0;
		defense = 1;
		attackRange = 3;
		attackDamage = 100;
		attackSpeed = 0.8;
		attackDelay.SetPreDelayRatio(0.3f);
		attackDelay.SetPostDelayRatio(0.1f);
		isMelee = false;
		break;
	case FreeLancerTemplateID::Assassin1:
		hpMax = 750;
		hpRegeneration = 1;
		mpMax = 100;
		mpRegeneration = 0;
		defense = 2;
		attackRange = 1;
		attackDamage = 30;
		attackSpeed = 0.6;
		attackDelay.SetPreDelayRatio(0.1f);
		attackDelay.SetPostDelayRatio(0.2f);
		isMelee = true;
		break;
	case FreeLancerTemplateID::Ranged2:
		hpMax = 700;
		hpRegeneration = 1;
		mpMax = 100;
		mpRegeneration = 0;
		defense = 1;
		attackRange = 1;
		attackDamage = 15;
		attackSpeed = 0.8;
		attackDelay.SetPreDelayRatio(0.2f);
		attackDelay.SetPostDelayRatio(0.1f);
		isMelee = false;
	case FreeLancerTemplateID::Melee2:
		hpMax = 1000;
		hpRegeneration = 2;
		mpMax = 100;
		mpRegeneration = 0;
		defense = 1;
		attackRange = 1;
		attackDamage = 300;
		attackSpeed = 1;
		attackDelay.SetPreDelayRatio(0.5f);
		attackDelay.SetPostDelayRatio(0.5f);
		isMelee = true;
		break;
	}
	hpNow = hpMax;
	mpNow = 0;
	attackDelay.SetDelay(attackSpeed);
}