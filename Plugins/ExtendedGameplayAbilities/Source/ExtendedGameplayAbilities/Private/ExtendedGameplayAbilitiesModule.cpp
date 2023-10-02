// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExtendedGameplayAbilitiesModule.h"

#include "ExtendedGameplayAbilitiesSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FExtendedGameplayAbilitiesModule"


void FExtendedGameplayAbilitiesModule::StartupModule()
{
	// register settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	SettingsModule->RegisterSettings(TEXT("Project"), TEXT("Plugins"), TEXT("ExtendedGameplayAbilities"),
	                                 NSLOCTEXT("ExtendedGameplayAbilities", "ExtendedGameplayAbilitiesSettingsName",
	                                           "Extended Gameplay Abilities"),
	                                 NSLOCTEXT("ExtendedGameplayAbilities", "ExtendedGameplayAbilitiesSettingsDescription",
	                                           "Project-wide settings for extended gameplay abilities."),
	                                 GetMutableDefault<UExtendedGameplayAbilitiesSettings>());
}

void FExtendedGameplayAbilitiesModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FExtendedGameplayAbilitiesModule, ExtendedGameplayAbilities)
