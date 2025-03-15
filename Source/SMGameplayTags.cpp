#include "SMGameplayTags.h"

#include "StereoMixLog.h"

// InitState
namespace StereoMix::Tags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_Spawned, "InitState.Spawned", "1: Actor/component has initially spawned and can be extended");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataAvailable, "InitState.DataAvailable", "2: All required data has been loaded/replicated and is ready for initialization");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_DataInitialized, "InitState.DataInitialized", "3: The available data has been initialized for this actor/component, but it is not ready for full gameplay");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InitState_GameplayReady, "InitState.GameplayReady", "4: The actor/component is fully ready for active gameplay");
}

// GamePhase
namespace StereoMix::Tags
{
	UE_DEFINE_GAMEPLAY_TAG(GamePhase_Message, "GamePhase.Message");

	UE_DEFINE_GAMEPLAY_TAG(GamePhase_Round_PreRound, "GamePhase.Round.PreRound");
	UE_DEFINE_GAMEPLAY_TAG(GamePhase_Round_InRound, "GamePhase.Round.InRound");
	UE_DEFINE_GAMEPLAY_TAG(GamePhase_Round_PostRound, "GamePhase.Round.PostRound");

	UE_DEFINE_GAMEPLAY_TAG(GamePhase_CharacterSelect_Selecting, "GamePhase.CharacterSelect.Selecting");
	UE_DEFINE_GAMEPLAY_TAG(GamePhase_CharacterSelect_PostSelect, "GamePhase.CharacterSelect.PostSelect");
}

// UI
namespace StereoMix::Tags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_FirstInitialized, "UI.FirstInitialized", "The UI has been initialized for the first time");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Game, "UI.Layer.Game", "Game Layer");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_GameMenu, "UI.Layer.GameMenu", "Game Menu Layer");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_MenuBackground, "UI.Layer.MenuBackground", "Menu Background Layer");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Menu, "UI.Layer.Menu", "Menu Layer");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Layer_Modal, "UI.Layer.Modal", "Modal Layer");
}

// Character
namespace StereoMix::Tags
{
	UE_DEFINE_GAMEPLAY_TAG(Character_Event_Hit, "Character.Event.Hit");

	UE_DEFINE_GAMEPLAY_TAG(Character_Message_AttributeChanged, "Character.Message.AttributeChanged");

	UE_DEFINE_GAMEPLAY_TAG(Character_DoNotLanding, "Character.DoNotLanding");
}

// Buff
namespace StereoMix::Tags
{
	UE_DEFINE_GAMEPLAY_TAG(Buff, "Buff");
	UE_DEFINE_GAMEPLAY_TAG(Buff_Slow, "Buff.Slow");
	UE_DEFINE_GAMEPLAY_TAG(Buff_Stun, "Buff.Stun");
	UE_DEFINE_GAMEPLAY_TAG(Buff_Heal, "Buff.Heal");
	UE_DEFINE_GAMEPLAY_TAG(Buff_Damage, "Buff.Damage");
	UE_DEFINE_GAMEPLAY_TAG(Buff_Immune, "Buff.Immune");
	UE_DEFINE_GAMEPLAY_TAG(Buff_Neutralize, "Buff.Neutralize");
}

// Stat
namespace StereoMix::Tags
{
	UE_DEFINE_GAMEPLAY_TAG(Stat, "Stat");
	UE_DEFINE_GAMEPLAY_TAG(Stat_CapturedTiles, "Stat.CapturedTiles");
	UE_DEFINE_GAMEPLAY_TAG(Stat_DamageDealt, "Stat.DamageDealt");
	UE_DEFINE_GAMEPLAY_TAG(Stat_DamageReceived, "Stat.DamageReceived");
	UE_DEFINE_GAMEPLAY_TAG(Stat_Kill, "Stat.Kill");
	UE_DEFINE_GAMEPLAY_TAG(Stat_Death, "Stat.Death");
	UE_DEFINE_GAMEPLAY_TAG(Stat_NoiseBreakUsage, "Stat.NoiseBreakUsage");
}

FGameplayTag StereoMix::Tags::FindTagByString(const FString& TagString, bool bMatchPartialString)
{
	const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

	if (!Tag.IsValid() && bMatchPartialString)
	{
		FGameplayTagContainer AllTags;
		Manager.RequestAllGameplayTags(AllTags, true);

		for (const FGameplayTag& TestTag : AllTags)
		{
			if (TestTag.ToString().Contains(TagString))
			{
				UE_LOG(LogStereoMix, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
				Tag = TestTag;
				break;
			}
		}
	}

	return Tag;
}
