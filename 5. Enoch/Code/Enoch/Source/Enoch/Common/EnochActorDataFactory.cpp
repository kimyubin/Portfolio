// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochActorDataFactory.h"
#include "EnochFreeLancerData.h"
#include "EnochProjectileData.h"
#include "EnochFieldCellData.h"

EnochActorDataFactory* EnochActorDataFactory::instance;

EnochActorDataFactory::EnochActorDataFactory()
{
}

EnochActorDataFactory::~EnochActorDataFactory()
{
}

void EnochActorDataFactory::BeginPlay()
{
	//용병 맵 초기화
	_freelancerMap = make_unique<vector<EnochFreeLancerData *>>();
	_freelancerQueue = make_unique<queue<int>>();

	//투사체 맵 초기화
	_projectileMap = make_unique<vector<EnochProjectileData*>>();
	_projectileQueue = make_unique<queue<int>>();
	instance = this;
}

int EnochActorDataFactory::SpawnFreeLancer()
{
	if (_freelancerQueue == nullptr || _freelancerMap == nullptr) return -1;
	//남는 인덱스번호 할당(없을경우, 마지막인덱스로 추가생성)
	auto SerialNumber = _freelancerQueue->empty() ? _freelancerMap->size() : _freelancerQueue->front();
	

	//데이터 생성
	auto newFreeLancer = new EnochFreeLancerData();
	newFreeLancer->SerialNumber = SerialNumber;

	//인덱스번호에 맞게 할당
	if (SerialNumber == _freelancerMap->size())
		_freelancerMap->emplace_back(newFreeLancer);
	else {
		_freelancerMap->at(SerialNumber) = newFreeLancer;
		_freelancerQueue->pop();
	}

	//할당이 제대로 되었는지 검증
	if (_freelancerMap->at(SerialNumber) == nullptr) {
		delete newFreeLancer;
		return -1;
	}
	
	//리턴
	return SerialNumber;
}

int EnochActorDataFactory::SpawnProjectile()
{
	if (_projectileQueue == nullptr || _projectileMap == nullptr) return -1;
	//남는 인덱스번호 할당(없을경우, 마지막 인덱스로 추가생성)
	auto SerialNumber = _projectileQueue->empty() ? _projectileMap->size() : _projectileQueue->front();
	
	//데이터 생성
	auto newProjectile = new EnochProjectileData();
	newProjectile->SerialNumber = SerialNumber;

	//인덱스번호에 맞게 할당
	if (SerialNumber == _projectileMap->size())
		_projectileMap->emplace_back(newProjectile);
	else {
		_projectileMap->at(SerialNumber) = newProjectile;
		_projectileQueue->pop();
	}
	//제대로 할당되었는지 검증
	if (_projectileMap->at(SerialNumber) == nullptr) {
		delete newProjectile;
		return -1;
	}

	return SerialNumber;
}

void EnochActorDataFactory::Release(EnochFreeLancerData* freeLancer)
{
	if (freeLancer == nullptr) return;
	auto SerialNumber = freeLancer->SerialNumber;
	auto field = EnochFieldData::GetData(freeLancer->location);
	if (field != nullptr) 
		field->SetFreeLancerOn(nullptr);
	
	_freelancerMap->at(SerialNumber) = nullptr;
	_freelancerQueue->push(SerialNumber);
	delete freeLancer;
}

void EnochActorDataFactory::Release(EnochProjectileData *projectile)
{
	if (projectile == nullptr) return;
	auto SerialNumber = projectile->SerialNumber;
	
	_projectileMap->at(SerialNumber) = nullptr;
	_projectileQueue->push(SerialNumber);
	delete projectile;
}

void EnochActorDataFactory::EndPlay()
{
	for (auto data : *_freelancerMap)
		if(data != nullptr) delete data;
	for (auto data : *_projectileMap)
		if (data != nullptr) delete data;

	instance = nullptr;
}