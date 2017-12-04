// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#include "GOAPer.h"
#include "GOAPerPrivatePCH.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ModuleInterface.h"
#include "ModuleManager.h"

#define LOCTEXT_NAMESPACE "FGOAPerModule"

void FGOAPerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// register settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "GOAPer",
			LOCTEXT("GOAPerSettingsName", "GOAPer"),
			LOCTEXT("GOAPerSettingsDescription", "Configure the GOAPer Plugin"),
			GetMutableDefault<UGOAPerSettings>()
			);

		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FGOAPerModule::HandleSettingsSaved);
		}
	}
}

void FGOAPerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// unregister settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "GOAPer");
	}
}

bool FGOAPerModule::HandleSettingsSaved()
{
	//RestartServices();

	return true;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGOAPerModule, GOAPer)