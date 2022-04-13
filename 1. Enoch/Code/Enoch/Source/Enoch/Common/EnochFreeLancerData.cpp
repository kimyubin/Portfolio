// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFreeLancerData.h"
#include "EnochFreeLancerState.h"
#include "EnochFiniteStateMachine.h"
#include "EnochFieldData.h"
#include "EnochActorDataFactory.h"
#include "EnochFieldCellData.h"
#include "EnochProjectileData.h"
#include "FLActs/EnochFreeLancerAct.h"
#include "EnochSimulator.h"
#include "SkillData.h"
#include <map>
#include "CSVParser.h"

#include "../Enoch.h"

#include "FLActs/FLJump.h"
#include "FLActs/FLIdle.h"
#include "FLActs/FLMove.h"
#include "FLActs/FLDelay.h"
#include "FLActs/FLMeleeAttack.h"
#include "FLActs/FLRangeAttack.h"

#include <memory>
using namespace std;

uint8 FreeLancerTemplateID::Num = 0;
uint8 AllianceID::Num = 0;

static map<uint8, shared_ptr<FreeLancerTemplate>> mFreeLancerTemplate;
static map<uint8, shared_ptr<AllianceTemplate>> mAllianceTemplate;

const FreeLancerTemplate* FreeLancerTemplate::GetFreeLancerTemplate(uint8 ID)
{
	auto it = mFreeLancerTemplate.find(ID); 
	if(it != mFreeLancerTemplate.end())
		return it->second.get();
	return nullptr;
}
const uint8 FreeLancerTemplate::GetFreeLancerModelID(uint8 TID)
{
	auto tmp = GetFreeLancerTemplate(TID);
	uint8 modelID = tmp ? tmp->modelID : 0;
	return modelID;
}

// column 열 순서 바뀔 경우 여기를 바꿔줘야함
enum FreeLancerColumn
{
	COLUMN_ID,
	COLUMN_GRADE,
	COLUMN_ALLIANCE,
	COLUMN_UNCHANGE,
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
	COLUMN_SKILL,
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
		
		elem->ID			= static_cast<uint8>(stoi(content[row_first+2][COLUMN_ID]));
		elem->name			= content[row_first + 1][COLUMN_ID];
		elem->grade			= stoi(first_content[COLUMN_GRADE]);
		elem->modelID		= stoi(content[row_first + 3][COLUMN_ID]);
		elem->buyPrice		= elem->grade;

		for(int j = 0; j < LEVEL_MAX; j++)
		{
			auto &row_tmp = content[row_first+j];
			if(row_tmp[COLUMN_ALLIANCE].empty())
				break;
			uint8 allianceID = stoi(row_tmp[COLUMN_ALLIANCE]);
			elem->alliance.push_back(allianceID);
		}
		
		elem->AiType		= static_cast<AIType>(stoi(content[row_first+3][COLUMN_UNCHANGE]));
		elem->attackRange	= stoi(first_content[COLUMN_UNCHANGE]);
		elem->attackDelayBefore	= stof(content[row_first+1][COLUMN_UNCHANGE]);
		elem->attackDelayAfter	= stof(content[row_first+2][COLUMN_UNCHANGE]);
		if (!content[row_first+4][COLUMN_UNCHANGE].empty())
			elem->shotSpeed		= stof(content[row_first+4][COLUMN_UNCHANGE]);
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

			if (!row_tmp[COLUMN_SKILL].empty())
				elem->shotSpeed		= stof(row_tmp[COLUMN_SKILL]);
			else
				elem->shotSpeed		= 0;
		}
		
		mFreeLancerTemplate.insert(make_pair(elem->ID, elem));
	}

	FreeLancerTemplateID::Num = mFreeLancerTemplate.size();
	
	return true;
}

void FreeLancerTemplate::UninitFreeLancerTemplate()
{
	mFreeLancerTemplate.clear();
	return;
}

FLBattleData::~FLBattleData()
{
	;
}

void FLBattleData::BeginPlay()
{
	
}

void FLBattleData::EndPlay()
{
	
	//actMap.release();
}

void FLBattleData::Act(float deltaTime)
{
	//동작이 없었던 경우 초기화
	if (actDelay <= 0) {
		EnochFiniteStateMachine::Decide(this);
	}
	//시간만큼 동작
	while (deltaTime > 0) {
		//EnochSimulator::logs.push({SimulateInfoType::Debug, 1,1,actDelay});
		//EnochSimulator::logs.push({ SimulateInfoType::Debug, 1,1,deltaTime });
		auto timeUsed = std::min(actDelay, deltaTime);
		if (GetState() != FreeLancerState::None && GetState() != FreeLancerState::Skill)
			if (actMap == nullptr) EnochSimulator::logs.push({ SimulateInfoType::Debug, 1,1,100 });
			else
			actMap->at((int)state)->Tick(deltaTime);
		actDelay -= timeUsed;
		deltaTime -= timeUsed;
		if (actDelay <= 0) {
			EnochFiniteStateMachine::Decide(this);
		}
	}
}

void FLBattleData::BeginSimulate()
{
	locationOnFight = location;
	//act.Init();
}

uint8 FLBattleData::GetTID()
{
	return templateID;
}

FreeLancerState FLBattleData::GetState()
{
	return state;
}

FLBattleData *FLBattleData::GetTarget()
{
	return EnochActorDataFactory::instance->GetFreeLancerData(_target);
}

void FLBattleData::SetTarget(FLBattleData* target)
{
	if (target == nullptr) _target = -1;
	else SetTarget(target->GetSerialNumber());
}

void FLBattleData::SetTarget(int SN_target)
{
	auto target = EnochActorDataFactory::instance->GetFreeLancerData(SN_target);
	_target = target == nullptr ? -1 : target->GetSerialNumber();
}

void FLBattleData::SetState(FreeLancerState stateIn) 
{
	if (state != stateIn)
		EnochSimulator::logs.push({ SimulateInfoType::ChangeState, SerialNumber, (int)stateIn, 0 });
	if(actMap != nullptr && state != FreeLancerState::None && stateIn != FreeLancerState::Dead)
		actMap->at((int)state)->End();
	
	state = stateIn;
	if (actMap != nullptr) {
		actMap->at((int)state)->Begin();
		actDelay = actMap->at((int)state)->length;
	}
	else
		actDelay = 0;
}

bool FLBattleData::CanUseSpell()
{
	return false;
}

bool FLBattleData::CanAttack()
{
	// 일단 원거리 고려 안함
	if (attackRange > 1)
		return true;
	return nextAttackTime <= EnochSimulator::simulateTime;
}

void FLBattleData::UpdateTime(float deltaTime)
{
	// 일단은 버프등을 위해 남겨두자
}

void FLBattleData::DoAttack(FLBattleData* attacker, FLBattleData* defender)
{
	if (!attacker || !defender)
		return;
	//데미지 계산
	auto damage = (int)(attacker->attackDamage - defender->defense);
	EnochSimulator::logs.push({ SimulateInfoType::MeleeAttack, attacker->SerialNumber, defender->SerialNumber, (int)damage });
	CalculateDamage(damage, defender);

	attacker->nextAttackTime = EnochSimulator::simulateTime + attacker->timeForOneAttack;
}

void FLBattleData::DoAttack(EnochProjectileData* attacker, FLBattleData* defender)
{
	if (!attacker || !defender)
		return;
	//데미지 계산
	auto damage = (int)(attacker->attackDamage - defender->defense);
	EnochSimulator::logs.push({ SimulateInfoType::PJAttack, attacker->SerialNumber, defender->SerialNumber, (int)damage });
	CalculateDamage(damage, defender);
}

void FLBattleData::CalculateDamage(int damage, FLBattleData* defender)
{
	if (damage >= defender->hpNow) {
		defender->hpNow = 0;
		//이후 체력 0에서 안죽는 스킬이 추가된다면 여기에 조건문 추가해야할듯

		//전투 중 사망
		defender->state = FreeLancerState::None;
		defender->SetState(FreeLancerState::Dead);
		defender->SetTarget(-1);

		//EnochFieldData::GetData(defender->locationOnFight)->SetFreeLancerOnFight(nullptr);
	}
	else
		defender->hpNow -= damage;
}

//return target address or nullptr. do not store return value

FLMove &FLBattleData::GetMove()
{
	return *static_pointer_cast<FLMove>(actMap->at((int)FreeLancerState::PreMove));
}


//기본스탯 템플릿 사용
void FLBattleData::InitStatus(uint8 TID, uint8 levelIn, int SN, bool isEnemy_)
{
	// 초기화
	SerialNumber = SN;
	templateID = TID;
	isEnemy = isEnemy_;
	if (isEnemy) direction = { -1,0 };
	else direction = { 1,0 };
	SetState(FreeLancerState::Idle);

	// 0 ~ 4 레벨 제한
	level = max<uint8>(min<uint8>(levelIn, FreeLancerTemplate::LEVEL_MAX), FreeLancerTemplate::LEVEL_MIN) - 1;
	
	const auto &tmp = FreeLancerTemplate::GetFreeLancerTemplate(TID);
	/////////////////////////
	// Initialize Actions
	actMap = make_unique< vector<shared_ptr<FreeLancerAct>> >();
	actMap->resize((int)FreeLancerState::Dead + 1, nullptr);
	actMap->at((int)FreeLancerState::Idle) = make_shared<FLIdle>();
	/*
	if (tmp->name.length() > 8 && tmp->name.substr(0, 8) == L"Assassin") {
		actMap->at((int)FreeLancerState::PreMove) = make_shared<FLJump>();
		canJump = true;
	}
	
	else {
	*/
		actMap->at((int)FreeLancerState::PreMove) = make_shared<FLMove>();
		canJump = false;
	//}
	actMap->at((int)FreeLancerState::PostMove) = make_shared<FLDelay>();
	if(tmp->attackRange > 1)
		actMap->at((int)FreeLancerState::PreAttack) = make_shared<FLRangeAttack>();
	else
		actMap->at((int)FreeLancerState::PreAttack) = make_shared<FLMeleeAttack>();
	actMap->at((int)FreeLancerState::PostAttack) = make_shared<FLDelay>();
	actMap->at((int)FreeLancerState::Dead) = make_shared<FLIdle>();
	actMap->at((int)FreeLancerState::Victory) = make_shared<FLIdle>();
	// ////////////////////////
	// 레벨 관련
	auto &levelData = tmp->levelData[level]; 
	hpMax = levelData.hp;
	hpRegeneration = levelData.hpGen;
	mpMax = levelData.mp;
	mpRegeneration = levelData.mpGen;
	defense = levelData.armor;
	attackDamage = levelData.atkDamage;
	attackSpeed = levelData.attackSpeed;
	
	// 레벨 무관
	attackRange = tmp->attackRange;
	actMap->at((int)FreeLancerState::Idle)->length = 0.1f;
	actMap->at((int)FreeLancerState::PreAttack)->length = tmp->attackDelayBefore;
	actMap->at((int)FreeLancerState::PostAttack)->length = tmp->attackDelayAfter;
	isMelee = attackRange == 1;
	
	hpNow = hpMax;
	mpNow = 0;
	for (auto& elem : *actMap)
		if (elem != nullptr && elem.get() != nullptr)
			elem->data = this;

	// 시뮬레이터 관련
	nextAttackTime = 0;
}

void FLBattleData::UpdateStat(StatFlag flag)
{
	const auto& tmp = FreeLancerTemplate::GetFreeLancerTemplate(templateID);
	auto& levelData = tmp->levelData[level];

	// 효과 별 버프/너프 모아서 계산해야함
	unordered_map<SkillEffect, list<float>> effects;

	// 동맹 부터 모으기
	for (uint8_t allianceID : tmp->alliance)
	{
		uint8_t AllianceLevel = EnochActorDataFactory::instance->GetAllianceLevel(isEnemy, allianceID);
		if (AllianceLevel == 0) continue;
		uint16_t skillID = AllianceTemplate::GetAllianceTemplate(allianceID)->skillID;
		const auto& skillTmp = SkillData::GetSkillTemplate(skillID);
		for (auto& effectPair : skillTmp->level[AllianceLevel].effects)
		{
			auto& effect = effectPair.first;
			float value = effectPair.second;
			if(effects.find(effect) == effects.end())
				effects[effect] = list<float>();
			effects[effect].push_back(value);
		}
	}

	//////////////////////////////
	// 효과 별 적용하기

	auto addIntValue = [&](int &value_, int default, SkillEffect effect)
	{
		value_ = default;
		if (effects.find(effect) != effects.end())
			for (float value : effects[effect])
				value_ += value;
	};

	auto addFloatValue = [&](float& value_, float default, SkillEffect effect)
	{
		value_ = default;
		if (effects.find(effect) != effects.end())
			for (float value : effects[effect])
				value_ += value;
	};

	auto multiFloatValue = [&](float& value_, float default, SkillEffect effect)
	{
		value_ = default;
		if (effects.find(effect) != effects.end())
			for (float value : effects[effect])
				value_ *= value;
	};

	if (flag & StatFlag::HPMAX) {
		int hpMaxBack = hpMax;
		addIntValue(hpMax, levelData.hp, SkillEffect::HP);

		// 현재 체력 변경
		if (hpMax == hpMaxBack)
			;
		else if (hpNow == hpMaxBack)
			hpNow = hpMax;
		else if (hpNow <= 0)
			;
		else
		{
			float ratio = hpMax / hpMaxBack;
			hpNow = hpNow * ratio;
		}
	}

	// MP를 변경할거 같지는 않지만 일단 구현
	if (flag & StatFlag::MPMAX) {	
		int mpMaxBack = mpMax;
		addIntValue(mpMax, levelData.mp, SkillEffect::MP);

		// 현재 MP 변경
		if (mpMax == mpMaxBack)
			;
		else if (mpNow == mpMaxBack)
			mpNow = mpMax;
		else if (mpNow <= 0)
			;
		else
		{
			float ratio = mpMax / mpMaxBack;
			mpNow = mpNow * ratio;
		}
	}

	if (flag & StatFlag::HPGEN) addFloatValue(hpRegeneration, levelData.hpGen, SkillEffect::HPGEN);
	if (flag & StatFlag::MPGEN) addFloatValue(mpRegeneration, levelData.mpGen, SkillEffect::MPGEN);
	if (flag & StatFlag::ATKDMG) addIntValue(attackDamage, levelData.atkDamage, SkillEffect::ATKDMG);

	if (flag & StatFlag::ATKSPD) {
		addIntValue(attackSpeed, (int)levelData.attackSpeed, SkillEffect::ATKSPD);
		const static float BaseAttackTime = 1.7f;
		timeForOneAttack = BaseAttackTime / (1 + attackSpeed / 100);
		// Attack Time = BAT / ( 1 + IAS/100 )
	}

	if (flag & StatFlag::ATKRNG) addIntValue(attackRange, tmp->attackRange, SkillEffect::ATKRNG);

	if (flag & StatFlag::DEF) {
		addIntValue(defense, levelData.armor, SkillEffect::ATKRNG);
		const static float ArmorFactor = 0.1;
		physicalReduce = ArmorFactor * defense / (1 + ArmorFactor * abs(defense));
	}

	if (flag & StatFlag::MAGREG) {
		// 마법 저항력은 0일때 100% 데미지를 얻으므로, 1에서 뺀다
		magicRegist = (1-levelData.magicArmor);
		if (effects.find(SkillEffect::MAGREG) != effects.end())
			for (float value : effects[SkillEffect::MAGREG])
				magicRegist *= (1-value);
	}

	// TODO : MAGDMG, PUREDMG 등은 아직 구현이 안됬으니 넘어가자
}

const AllianceTemplate* AllianceTemplate::GetAllianceTemplate(uint8 ID)
{
	auto it = mAllianceTemplate.find(ID);
	if (it != mAllianceTemplate.end())
		return it->second.get();
	return nullptr;
}

bool AllianceTemplate::InitAllianceTemplate(wstring path)
{
	const static wstring filename = L"AllianceTemplateData.csv";
	wstring path_data = path + L"/" + filename;

	CSVParser parser(path_data);
	if (!parser.success)
	{
		UE_LOG(Enoch, Log, TEXT("Success : %s"), parser.success ? TEXT("TRUE") : TEXT("FALSE"));
		return false;
	}

	const auto& content = parser.content;
	const unsigned size_data = content.size();

	if ( !mAllianceTemplate.empty() )
		AllianceTemplate::UninitAllianceTemplate();

	// 첫줄은 헤더라 무시, idx 를 1부터 시작
	unsigned idx = 1;
	while (idx < size_data)
	{
		shared_ptr<AllianceTemplate> elem(new AllianceTemplate());

		unsigned tail = idx;
		while (++tail < size_data && content[tail][0].empty())
			;
		const uint8 level = tail - idx;
		elem->maxLevel = level;
		elem->ID = static_cast<uint8>(stoi(content[idx][0]));
		elem->name = content[idx][1];
		elem->skillID = static_cast<uint16>(stoi(content[idx][2]));
		for (uint8 i = 0; i < level; i++)
		{
			const uint8 num = static_cast<uint8>(stoi(content[idx+i][3]));
			elem->level.push_back(num);
		}

		mAllianceTemplate.insert(make_pair(elem->ID, elem));
		idx += level;
	}

	AllianceID::Num = mAllianceTemplate.size();
	return true;
}

void AllianceTemplate::UninitAllianceTemplate()
{
	mAllianceTemplate.clear();
	return;
}
