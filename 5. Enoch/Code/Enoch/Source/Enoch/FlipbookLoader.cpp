// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipbookLoader.h"

TMap<FreeLancerTemplateID, TSharedPtr<TMap<FString, TSharedPtr<UPaperFlipbook>>>> AFlipbookLoader::FLFlipbookMap;
TMap<ProjectileTemplateID, TSharedPtr<TMap<FString, TSharedPtr<UPaperFlipbook>>>> AFlipbookLoader::PJFlipbookMap;
bool AFlipbookLoader::isInitialized = false;

// Sets default values
AFlipbookLoader::AFlipbookLoader()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!isInitialized)
		InitFlipbookMap();
}

// Called when the game starts or when spawned
void AFlipbookLoader::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlipbookLoader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Flipbook의 Pathname 규격이 고정되어있다는 가정 하에 만든 함수. 바뀌게 된다면 이 함수도 수정해야함
//에셋을 읽어들이는건 생성자에서만 실행해야함! 따라서 이 함수는 생성자 스택에서 호출
void AFlipbookLoader::InitFlipbookMap()
{
/*
	define문 수정할일 있을시 참고
	EXAMPLE(Type) 호출시
	##Type을 사용하면 그냥 Type이랑 동일한 동작이지만, 다른문자나 토큰과 띄어쓰기없이 붙어있어도 동작하기때문에 함수호출등에 활용 가능. 단, 맥에서 컴파일 안되는것 확인되어 사용 안함
	#를 사용하면 문자열로 변함. "Type"이라는 문자열로 바꾸고싶을 때 사용
*/
#define SPRITE_FOLDER() "/Game/DataForCli/Sprite/"
#define FLIP_PATH(Type, State) TEXT(SPRITE_FOLDER()#Type"/"#Type"_"#State"."#Type"_"#State)
#define ADD_FLIP(Type, State) book->Add(""#State"", LoadFlipbook(TCHAR_TO_WCHAR(FLIP_PATH( Type , State ))));
#define REGISTER_FLIP(Type) \
	{ \
		auto book = MakeShared<TMap<FString, TSharedPtr<UPaperFlipbook>>>(); \
		ADD_FLIP(Type, Idle); \
		ADD_FLIP(Type, Attack); \
		ADD_FLIP(Type, Move); \
		ADD_FLIP(Type, Victory); \
		ADD_FLIP(Type, Dead); \
		FLFlipbookMap.Add(FreeLancerTemplateID::Type, book); \
	}

#define REGISTER_FLIP_PJ(Type) \
	{ \
		auto book = MakeShared<TMap<FString, TSharedPtr<UPaperFlipbook>>>(); \
		ADD_FLIP(Type, Idle); \
		ADD_FLIP(Type, Attack); \
		PJFlipbookMap.Add(ProjectileTemplateID::Type, book); \
	}

	/////////////////////////////////////////////// 
	FLFlipbookMap = TMap<FreeLancerTemplateID, TSharedPtr<TMap<FString, TSharedPtr<UPaperFlipbook>>>>();
	PJFlipbookMap = TMap<ProjectileTemplateID, TSharedPtr<TMap<FString, TSharedPtr<UPaperFlipbook>>>>();
	REGISTER_FLIP(Melee1);
	REGISTER_FLIP(Melee2);
	REGISTER_FLIP(Ranged1);
	REGISTER_FLIP(Ranged2);
	REGISTER_FLIP(Assassin1);

	REGISTER_FLIP_PJ(RedDot);

	isInitialized = true;
}

//Flipbook 한개를 읽어들이는 함수.
//에셋을 읽어들이는건 생성자에서만 실행해야함! 따라서 이 함수는 생성자 스택에서 호출
TSharedPtr<UPaperFlipbook> AFlipbookLoader::LoadFlipbook(const wchar_t* path)
{
    auto flipbook = ConstructorHelpers::FObjectFinder<UPaperFlipbook>(WCHAR_TO_TCHAR(path));
	if (flipbook.Succeeded()) {
		return TSharedPtr<UPaperFlipbook>(flipbook.Object);
	}
	else {
		return nullptr;
	}
}

TSharedPtr<TMap<FString, TSharedPtr<UPaperFlipbook>>> AFlipbookLoader::GetFlipbook(FreeLancerTemplateID type)
{	
	return FLFlipbookMap.Contains(type) ? FLFlipbookMap[type] : nullptr;
}

TSharedPtr<TMap<FString, TSharedPtr<UPaperFlipbook>>> AFlipbookLoader::GetFlipbook(ProjectileTemplateID type)
{
	return PJFlipbookMap.Contains(type) ? PJFlipbookMap[type] : nullptr;
}