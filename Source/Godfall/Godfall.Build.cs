// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Godfall : ModuleRules
{
	public Godfall(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		//PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "AIModule", "NavigationSystem", "GameplayTasks" });
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"UMG", 
			"NavigationSystem", 
			"AIModule", 
			"GameplayTasks",
			"Niagara",
		});
	}
}
