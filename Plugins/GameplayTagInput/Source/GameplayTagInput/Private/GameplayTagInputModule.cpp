// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayTagInputModule.h"


DEFINE_LOG_CATEGORY(LogGameplayTagInput)

#define LOCTEXT_NAMESPACE "FGameplayTagInputModule"

void FGameplayTagInputModule::StartupModule()
{
}

void FGameplayTagInputModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGameplayTagInputModule, GameplayTagInput)
