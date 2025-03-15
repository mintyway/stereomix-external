// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "SMAN_PlayFaceAnimation.generated.h"

USTRUCT(BlueprintType)
struct FPlayFaceAnimationMaterialIndices
{
	GENERATED_BODY()

	bool IsValid() const { return MouseMaterialIndex >= 0 && EyesMaterialIndex >= 0 && EyesOverlayMaterialIndex >= 0; }

	UPROPERTY(EditAnywhere)
	int32 MouseMaterialIndex = -1;

	UPROPERTY(EditAnywhere)
	int32 EyesMaterialIndex = -1;

	UPROPERTY(EditAnywhere)
	int32 EyesOverlayMaterialIndex = -1;
};

USTRUCT(BlueprintType)
struct FSpriteData
{
	GENERATED_BODY()

	bool IsValid() const { return SpriteImage && GetFrameCount() > 0; }

	int32 GetFrameCount() const { return Row * Column; };

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture> SpriteImage;

	UPROPERTY(EditAnywhere)
	int32 Row = 0;

	UPROPERTY(EditAnywhere)
	int32 Column = 0;
};

inline TMap<TWeakObjectPtr<USkeletalMeshComponent>, FTimerHandle> GlobalFaceAnimationTimerHandleMap;

/**
 * 
 */
UCLASS(Abstract)
class STEREOMIX_API USMAN_PlayFaceAnimation : public UAnimNotify
{
	GENERATED_BODY()

public:
	USMAN_PlayFaceAnimation();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	void OnUpdate(TWeakObjectPtr<const UWorld> World, TWeakObjectPtr<USkeletalMeshComponent> MeshComp, int32 CurrentFrame, int32 TotalFrameCount);

	UPROPERTY(EditDefaultsOnly, Category = "Design|Material")
	TObjectPtr<UMaterialInterface> ParentMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Design|Material")
	TObjectPtr<UMaterialInterface> ParentOverlayMaterial;

	/** 스프라이트를 재생할 속도입니다. */
	UPROPERTY(EditInstanceOnly, Category = "Design", DisplayName = "재생속도")
	FFrameRate FrameRate = FFrameRate(30, 1);

	/** 재생할 스프라이트 데이터입니다. */
	UPROPERTY(EditInstanceOnly, Category = "Design", DisplayName = "스프라이트 데이터")
	FSpriteData SpriteData;

	/** 오버레이 머티리얼에 사용될 컬러값입니다. */
	UPROPERTY(EditInstanceOnly, Category = "Design", DisplayName = "오버레이 머티리얼 색상")
	FLinearColor OverlayColor = FLinearColor(1.0, 1.0, 1.0, 1.0);

	/** 스프라이트가 재생되야할 머티리얼 인덱스들입니다. */
	UPROPERTY(EditInstanceOnly, Category = "Design", DisplayName = "사용할 머티리얼 인덱스")
	FPlayFaceAnimationMaterialIndices MaterialIndices;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DefaultMID;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> OverlayMID;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> MIDs;

private:
	static const FName FrameParameterName;
	static const FName RowParameterName;
	static const FName ColumnParameterName;
	static const FName SpriteParameterName;
	static const FName ColorParameterName;
};
