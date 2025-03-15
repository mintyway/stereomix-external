// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NativeGameplayTags.h"
#include "SMCharacterFeedAvatarDataAsset.generated.h"

// UENUM()
// enum class ESMCharacterFeedAvatarState
// {
// 	Idle,
// 	Attack,
// 	Damaged,
// 	NoiseBreak
// };

enum class ESMTeam : uint8;

// namespace StereoMix::Tags
// {
// 	STEREOMIX_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Feed_State_Idle);
// 	STEREOMIX_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Feed_State_Attack);
// 	STEREOMIX_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Feed_State_Damaged);
// 	STEREOMIX_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Feed_State_NoiseBreak);
// }

struct FGameplayTag;

USTRUCT(BlueprintType, DisplayName = "StereoMix Character Feed Avatar Texture Set")
struct FSMCharacterFeedAvatarTexture
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Feed|Avatar")
	TSoftObjectPtr<UTexture2D> AvatarTexture;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Feed|Avatar", meta=(Categories = "Event.Character"))
	FGameplayTagContainer EventTags;
};

USTRUCT(BlueprintType, DisplayName = "StereoMix Character Feed Avatar Texture Set")
struct FSMCharacterFeedAvatarTextureSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Feed|Avatar")
	TObjectPtr<UTexture2D> IdleTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Feed|Avatar")
	TObjectPtr<UTexture2D> AttackTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Feed|Avatar")
	TObjectPtr<UTexture2D> HitTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Feed|Avatar")
	TObjectPtr<UTexture2D> NoiseBreakTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Feed|Avatar")
	TObjectPtr<UTexture2D> NeutralizeTexture;
};

UCLASS(BlueprintType, DisplayName = "StereoMix Character Feed Avatar Data Asset")
class STEREOMIX_API USMCharacterFeedAvatarDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Feed|Avatar")
	TMap<ESMTeam, FSMCharacterFeedAvatarTextureSet> AvatarTextureSets;
};
