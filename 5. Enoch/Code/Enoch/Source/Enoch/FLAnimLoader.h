// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "GameFramework/Actor.h"
#include "Common/EnochFreeLancerdata.h"
#include "FLAnimLoader.generated.h"

UCLASS()
class ENOCH_API AFLAnimLoader : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFLAnimLoader();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	static AFLAnimLoader* instance;
	static USkeletalMesh *GetMesh(FreeLancerTemplateID templateID);
	static TSubclassOf<UAnimInstance> *GetAnim(FreeLancerTemplateID templateID);

private :
	//메쉬 정보들
	static TMap<FreeLancerTemplateID, USkeletalMesh*> Meshes;
	static TMap<FreeLancerTemplateID, TSubclassOf<UAnimInstance>> Anims;
	static bool LoadModelFiles();	//리턴값 의미없음
	static void LoadMeshFile(const wchar_t* path, enum class FreeLancerTemplateID type);
	static void LoadAnimFile(const wchar_t* path, enum class FreeLancerTemplateID type);

};
