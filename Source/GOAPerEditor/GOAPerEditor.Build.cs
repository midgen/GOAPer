// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class GOAPerEditor : ModuleRules
{
	public GOAPerEditor(ReadOnlyTargetRules Target) : base(Target)
    {

    PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine",  "GOAPer", "InputCore"});

    PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "PropertyEditor", "EditorStyle", "UnrealEd", "GraphEditor", "BlueprintGraph" });


    PublicIncludePaths.AddRange(
    new string[] {
                Path.Combine(ModuleDirectory, "Public")
        // ... add public include paths required here ...
    });


    PrivateIncludePaths.AddRange(
    new string[] {
            Path.Combine(ModuleDirectory, "Private")
    });
        

    PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

    }
}
