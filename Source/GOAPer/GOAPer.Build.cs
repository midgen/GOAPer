// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GOAPer : ModuleRules
{
	public GOAPer(TargetInfo Target)
	{

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Slate", "SlateCore", "InputCore", "AIModule", "UMG" });

	PrivateIncludePathModuleNames.AddRange(
		new string[] {
			"Settings",
        }
	);
	
	PrivateIncludePaths.AddRange(
			new string[] {
				"GOAPer/Private",
			}
	);
	
	PrivateIncludePaths.AddRange(
			new string[] {
				"GOAPer/Public/Shared",
			}
	);

    }
}
