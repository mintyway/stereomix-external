// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Data/Ability/SMActiveAbility.h"
#include "SMActiveAbilityDisplaySlotData.generated.h"

struct FGameplayTag;
class USMGameplayAbility;
class UInputAction;

USTRUCT(BlueprintType)
struct FSMActiveAbilityDisplayEntryData
{
	GENERATED_BODY()

	/** 어빌리티의 액티브 타입 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Overlay Definition")
	ESMActiveAbilityType ActiveType{};

	/** 어빌리티와 대응하는 인풋 액션 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Overlay Definition")
	TObjectPtr<const UInputAction> InputAction;

	/** 비활성화 시 게이지 표시 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Overlay Definition")
	bool bHasGauge = false;

	/** 게이지 어트리뷰트 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Overlay Definition", meta=(EditCondition="bHasGauge"))
	FGameplayAttribute GaugeAttribute;

	/** 최대 게이지 어트리뷰트 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Overlay Definition", meta=(EditCondition="bHasGauge"))
	FGameplayAttribute MaxGaugeAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Overlay Definition")
	bool bHideOnDeactivate = false;

	int32 GetInputID() const
	{
		return static_cast<int32>(ActiveType);
	}
};

UCLASS(BlueprintType)
class STEREOMIX_API USMActiveAbilityDisplaySlotData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// 한 슬롯에 등록할 어빌리티 정보 모음입니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Overlay Definition")
	TArray<FSMActiveAbilityDisplayEntryData> Entries;

	const FSMActiveAbilityDisplayEntryData* FindEntryDataByInputID(int32 InputID) const;
};
