// Fill out your copyright notice in the Description page of Project Settings.
#include "EnochGameInstance.h"

#include "Enoch.h"

UEnochGameInstance::UEnochGameInstance()
{

}
void UEnochGameInstance::Init()
{
	Super::Init();

	InitData();
	
	commander = NewObject<UCommanderWrapper>();
	commander->InitCommander();
	bSoloMode = true;
	SelectedFreeLancerInfo = FSelectedFreelancer();
	commander->ReqUserInfo();
}

void UEnochGameInstance::Shutdown()
{
	UninitData();
	
	Super::Shutdown();
}

void UEnochGameInstance::InitData()
{
	// 에디터, 릴리즈 구분해야함
	FString cwd = FString(WCHAR_TO_TCHAR(GetEnochDataPath().c_str()));
	
	UE_LOG(Enoch, Log, TEXT("Start Init Data - Working Dir : %s"), *cwd);
	
	//Todo: mac os 컴파일 커밋에서 문제가 된부분
	//if (!FreeLancerTemplate::InitFreeLancerTemplate(TCHAR_TO_WCHAR(&cwd)) )
	if (!FreeLancerTemplate::InitFreeLancerTemplate(*cwd) )
	{
		UE_LOG(Enoch, Log, TEXT("FreeLancer Template Data Load Fail"));
	}
	UE_LOG(Enoch, Log, TEXT("End Init Data"));
}

void UEnochGameInstance::UninitData()
{
	UE_LOG(Enoch, Log, TEXT("Start Uninit Data"));
}
void UEnochGameInstance::SelectedFreelancerUpdate(FSelectedFreelancer val)
{	
	SelectedFreeLancerInfo = val;
	MyPlayMenuUI->SetSelectedFreeLancerMat();
}
void UEnochGameInstance::DeselectedFLUpdate()
{
	//내용물 비움
	SelectedFreeLancerInfo = FSelectedFreelancer();
	//이미지 업데이트
	MyPlayMenuUI->SetSelectedFreeLancerMat();
}

void UEnochGameInstance::LiftFreeLancerInField(const FVector2D &CellLocation)
{
	FSelectedFreelancer Select = FSelectedFreelancer(CellLocation,commander->GetFieldFreeLancerList(CellLocation).FLTemplateID);
	SelectedFreelancerUpdate(Select);
}
