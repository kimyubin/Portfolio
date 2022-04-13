// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <string>

DECLARE_LOG_CATEGORY_EXTERN(Enoch, Log, All);
#define ENLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define ENLOG_S(Verbosity) UE_LOG(Enoch, Verbosity, TEXT("%s"), *ENLOG_CALLINFO)
#define ENLOG(Verbosity, Format, ...) UE_LOG(Enoch, Verbosity, TEXT("%s%s"), *ENLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define ENCHECK(Expr, ...) { if(!(Expr)) {ENLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; } }

std::wstring GetEnochDataPath();