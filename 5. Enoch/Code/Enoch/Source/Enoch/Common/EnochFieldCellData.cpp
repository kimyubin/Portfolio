// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFieldCellData.h"
#include "EnochFreeLancerData.h"

EnochFieldCellData::~EnochFieldCellData()
{
}


void EnochFieldCellData::SetFreeLancerOn(EnochFreeLancerData* arg)
{
	FreeLancerOn = arg;
	if(arg != nullptr) FreeLancerOn->location = location;
}

void EnochFieldCellData::SetFreeLancerOnFight(EnochFreeLancerData* arg)
{
	FreeLancerOnFight = arg;
}