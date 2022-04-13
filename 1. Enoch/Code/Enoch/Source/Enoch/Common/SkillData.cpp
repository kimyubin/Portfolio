#include "SkillData.h"
#include "CSVParser.h"

// 컴파일 전에 uint16 테스트하기

#include <map>
#include <memory>

#define	MACRO_MAP_ELEM(STRUCT, STR)	{L#STR, STRUCT::STR}

#define MACROEFFECT(STR) MACRO_MAP_ELEM(SkillEffect, STR)
static const map<wstring, SkillEffect> EffectInit =
{
	MACROEFFECT(HP),
	MACROEFFECT(HPGEN),
	MACROEFFECT(MP),
	MACROEFFECT(MPGEN),
	MACROEFFECT(ATKDMG),
	MACROEFFECT(ATKSPD),
	MACROEFFECT(ATKRNG),
	MACROEFFECT(DEF),
	MACROEFFECT(MAGREG),
	MACROEFFECT(MAGDMG),
	MACROEFFECT(PUREDMG),
};

#define MACROSHAPE(STR) MACRO_MAP_ELEM(SkillShape, STR)
static const map<wstring, SkillShape> ShapeInit =
{
	MACROSHAPE(Passive),
};

#define MACROTARGET(STR) MACRO_MAP_ELEM(SkillTarget, STR)
static const map<wstring, SkillTarget> TargetInit =
{
	MACROTARGET(Ally_All),
	MACROTARGET(Enemy_All),
};

#define MACRODISPEL(STR) MACRO_MAP_ELEM(SkillDispel, STR)
static const map<wstring, SkillDispel> DispelInit =
{
	MACRODISPEL(None),
};

static map<uint16_t, shared_ptr<SkillData>> mSkillTemplate;
uint16_t SkillData::Num = 0;

bool SkillData::InitSkillTemplate(wstring path)
{
	const static wstring filename = L"SkillData.csv";
	wstring path_data = path + L"/" + filename;

	CSVParser parser(path_data);
	if (!parser.success)
		return false;

	const auto& content = parser.content;
	const unsigned size_data = content.size();

	if (!mSkillTemplate.empty())
		SkillData::UninitSkillTemplate();

	// 첫줄은 헤더라 무시, idx 를 1부터 시작
	unsigned idx = 1;
	while (idx < size_data)
	{
		shared_ptr<SkillData> elem(new SkillData());

		unsigned tail = idx;
		while (++tail < size_data && content[tail][0].empty())
			;
		const uint8_t level = tail - idx;
		elem->maxLevel = level;
		elem->ID = static_cast<uint16_t>(stoi(content[idx][0]));
		elem->name = content[idx][1];

#define MACROFIND(MAP, CONTENT, DEFAULT) MAP.find(CONTENT) != MAP.end() ? MAP.find(CONTENT)->second : DEFAULT

		elem->shape = MACROFIND(ShapeInit, content[idx][2], SkillShape::Passive);
		//stoi(content[idx][3]; //사용방법 컬럼은 일단 보류
		elem->target = MACROFIND(TargetInit, content[idx][4], SkillTarget::Ally_All);
		elem->dispel = MACROFIND(DispelInit, content[idx][5], SkillDispel::None);

		for (uint8_t i = 0; i < level; i++)
		{
			const SkillEffect effect = MACROFIND(EffectInit, content[idx + i][6], SkillEffect::HP);
			const float value = stof(content[idx + i][7]);
			SkillLevelData data;
			data.effects.push_back(make_pair(effect, value));
			elem->level.push_back(data);
		}

		mSkillTemplate.insert(make_pair(elem->ID, elem));
		idx += level;
	}

	SkillData::Num = mSkillTemplate.size();
	return true;
}

void SkillData::UninitSkillTemplate()
{
	mSkillTemplate.clear();
	return;
}

const SkillData* SkillData::GetSkillTemplate(uint16_t ID)
{
	auto it = mSkillTemplate.find(ID);
	if (it != mSkillTemplate.end())
		return it->second.get();
	return nullptr;
}