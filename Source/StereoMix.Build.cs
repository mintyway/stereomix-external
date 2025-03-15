// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StereoMix : ModuleRules
{
	public StereoMix(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.Add("StereoMix");

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			"UMG",
			"AdvancedWidgets",
			"CommonUI",
			"CommonInput",
			"AnimGraphRuntime",
			"CADKernel",
			"LevelSequence",
			"MovieScene",
			"Niagara",
			"Json",
			"JsonUtilities",
			"HTTP",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"DeveloperSettings",
			"ModularGameplay",
			"ModularGameplayActors",
			"GameplayMessageRuntime",
			"CommonGame",
			"CommonUser",
			"CommonLoadingScreen",
			"AsyncMixin",
			"ActorInit",
			"GameSettings"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Slate",
			"SlateCore",
			"NetCore",
			"ApplicationCore",
			"EngineSettings",
			"KawaiiPhysics",
			"StructUtils",
			"MediaAssets",
			"ControlFlows",
			"Serialization",
			"UIExtension",
			"RHI",
			"AudioMixer",
			"AudioModulation",
			"FMODStudio",
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"CoreOnline",
			"StereoMixSDK",
			"EOSSDK",
			"EOSShared",
			"OnlineSubsystem",
			"OnlineSubsystemEOS",
			"OnlineSubsystemUtils"
		});

		DynamicallyLoadedModuleNames.AddRange(new[]
		{
			"OnlineSubsystemSteam"
		});

		SetupIrisSupport(target);
		SetupGameplayDebuggerSupport(target);
	}
}