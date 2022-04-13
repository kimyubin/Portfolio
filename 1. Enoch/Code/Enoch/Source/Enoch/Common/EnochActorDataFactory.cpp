// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochActorDataFactory.h"
#include "EnochFreeLancerData.h"
#include "EnochProjectileData.h"
#include "EnochFieldCellData.h"

EnochActorDataFactory* EnochActorDataFactory::instance;

EnochActorDataFactory::EnochActorDataFactory()
{
	actorID = 0;
}

EnochActorDataFactory::~EnochActorDataFactory()
{
}

void EnochActorDataFactory::BeginPlay()
{
	_FreeLancerMap.clear();
	_ProjectileMap.clear();
	allianceAlly.clear();
	allianceEnemy.clear();
	_SerialNumber_FreeLancer = 1;
	_SerialNumber_Projectile = 1;
	instance = this;
}

int EnochActorDataFactory::SpawnFreeLancer()
{
	_FreeLancerMap[_SerialNumber_FreeLancer] = std::make_shared<FLBattleData>(_SerialNumber_FreeLancer);
	return _SerialNumber_FreeLancer++;
}

int EnochActorDataFactory::SpawnProjectile()
{
	_ProjectileMap[_SerialNumber_Projectile] = std::make_shared<EnochProjectileData>(_SerialNumber_Projectile);
	return _SerialNumber_Projectile++;
}

void EnochActorDataFactory::DeleteFreeLancer(int SerialNumber)
{
	_FreeLancerMap.erase(SerialNumber);
}


void EnochActorDataFactory::DeleteProjectile(int SerialNumber)
{
	_ProjectileMap.erase(SerialNumber);
}

void EnochActorDataFactory::EndPlay()
{
	_FreeLancerMap.clear();
	_ProjectileMap.clear();
	allianceAlly.clear();
	allianceEnemy.clear();

	instance = nullptr;
}

void EnochActorDataFactory::UpdateAllianceAll()
{
	allianceAlly.clear();
	allianceEnemy.clear();

	// 삽입
	for (auto& elem : _FreeLancerMap)
	{
		auto& data = elem.second;
		auto& alliMap = data->isEnemy ? allianceEnemy : allianceAlly;

		auto tmp = FreeLancerTemplate::GetFreeLancerTemplate(data->GetTID());
		if (tmp == nullptr)
			continue;
		for (auto& alliID : tmp->alliance)
		{
			if (alliMap.find(alliID) == alliMap.end())
			{
				// Level, MemberSet
				alliMap[alliID] = make_pair(0, make_shared<set<uint8_t>>() );
			}

			alliMap[alliID].second->insert(tmp->ID);
		}
	}

	// 멤버 수 확인하고 레벨 설정
	auto levelSet = [](auto &alliMap, wstring& str)->void
	{
		for (auto& elem : alliMap)
		{
			auto alliID = elem.first;
			uint8_t count = elem.second.second->size();
			auto alliTmp = AllianceTemplate::GetAllianceTemplate(alliID);
			uint8_t level = 0;
			for (auto& lv : alliTmp->level)
			{
				if (count < lv)
					break;
				level++;
			}
			elem.second.first = level;
			if (level > 0)
			{
				if (!str.empty()) str += '\n';
				str += alliTmp->name + L"(" + to_wstring(level) + L")";
			}
		}
	};

	strAlly.clear();
	strEnemy.clear();
	levelSet(allianceAlly, strAlly);
	levelSet(allianceEnemy, strEnemy);
}

void EnochActorDataFactory::UpdateFLAll() {
	for (auto &fl : _FreeLancerMap) {
		fl.second->UpdateStat();
	}

}

uint8_t EnochActorDataFactory::GetAllianceLevel(bool isEnemy, uint8_t allianceID)
{
	auto alli = this->GetAlliance(isEnemy);
	if (alli.find(allianceID) == alli.end())
		return 0;
	return alli.find(allianceID)->second.first;
}