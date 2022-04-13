#pragma once
#include <vector>
#include <string>
using namespace std;

// 컴파일 전에 uint16 테스트하기

// 스킬 액티브/패시브 효과 추가시 여기에 추가
// uint16 : 0 ~ 65535
enum class SkillEffect : uint16_t
{
	// passive
	PASSIVE_START = 0,
	HP = PASSIVE_START,
	HPGEN,
	MP,			// 최대 MP를 바꾸는 일은 없을거같지만... 일단 추가
	MPGEN,
	ATKDMG,
	ATKSPD,
	ATKRNG,
	DEF,
	MAGREG,		// 마법 저항력
	MAGDMG,		// 마법 공격력
	PUREDMG,	// 순수데미지
	PASSIVE_END,
};

// 스킬 형태 : 패시브인지, 평타강화인지, 타겟팅인지...
enum class SkillShape : uint8_t
{
	Passive,
	Num_MAX,
};

// 스킬 대상 : 아군 전체, 아군 대상, 적군 전체, ...
enum class SkillTarget : uint8_t
{
	Ally_All,
	Enemy_All,
	Num_MAX,
};

// 정화 여부
enum class SkillDispel : uint8_t
{
	None,
	Num_MAX,
};

class SkillLevelData
{
public:
	vector<pair<SkillEffect, float>> effects;
};

class SkillData
{
public:
	uint16_t ID;
	wstring name;
	uint8_t maxLevel;
	SkillShape shape;
	SkillTarget target;
	SkillDispel dispel;
	vector<SkillLevelData> level;

	static uint16_t Num;
	static bool InitSkillTemplate(wstring path);
	static const SkillData* GetSkillTemplate(uint16_t ID);
	static void UninitSkillTemplate();
};
