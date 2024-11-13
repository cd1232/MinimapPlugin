// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Minimap : ModuleRules
{
	public Minimap(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"CoreUObject", 
				"Engine", 
				"UMG",
				"SlateCore",
				"SourceControl",
				"UnrealEd",
				"AssetTools"
				// ... add private dependencies that you statically link with here ...	
			}
			);
	}
}
