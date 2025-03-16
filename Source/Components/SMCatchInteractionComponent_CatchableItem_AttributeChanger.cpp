// Copyright Surround, Inc. All Rights Reserved.


#include "SMCatchInteractionComponent_CatchableItem_AttributeChanger.h"

#include "Engine/OverlapResult.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actors/Items/SMCatchableItem_AttributeChanger.h"
#include "Actors/Tiles/SMTile.h"
#include "Components/BoxComponent.h"
#include "Utilities/SMCollision.h"


USMCatchInteractionComponent_CatchableItem_AttributeChanger::USMCatchInteractionComponent_CatchableItem_AttributeChanger()
{
}

void USMCatchInteractionComponent_CatchableItem_AttributeChanger::OnSpecialActionPerformed(AActor* Instigator, ESpecialAction InSpecialAction)
{
}

void USMCatchInteractionComponent_CatchableItem_AttributeChanger::OnSpecialActionEnded(AActor* Instigator, ESpecialAction InSpecialAction, float InMagnitude, TSubclassOf<UGameplayEffect> DamageGE, float DamageAmount)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	switch (InSpecialAction)
	{
		case ESpecialAction::Smash:
		{
			InternalOnSmashedEnded(Instigator, InMagnitude);
			break;
		}
	}
}

void USMCatchInteractionComponent_CatchableItem_AttributeChanger::InternalOnSmashedEnded(AActor* Instigator, float InMagnitude)
{
	ASMCatchableItem_AttributeChanger* SourceItem = Cast<ASMCatchableItem_AttributeChanger>(SourceActor);
	if (!ensureAlways(SourceItem))
	{
		return;
	}

	// 아이템을 놓아줍니다.
	InternalOnCaughtReleased(Instigator);

	// 아이템의 바로아래 타일을 트리거하고 이 타일을 기준으로 스매시의 크기만큼의 타일을 골라내 소스액터에 저장합니다.
	ASMTile* TriggeredTile = TileTrigger(Instigator);
	if (TriggeredTile)
	{
		const FVector TileLocation = TriggeredTile->GetTileLocation();

		// FX 재생을 위해 약간 어긋난 아이템의 위치를 보정합니다.
		SourceItem->SetActorLocation(TileLocation);

		SaveTriggeredTilesBySmash(TriggeredTile, InMagnitude);

		// 스매시 이펙트를 재생합니다.
		UAbilitySystemComponent* InstigatorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Instigator);
		if (ensureAlways(InstigatorASC))
		{
			FGameplayCueParameters GCParams;
			GCParams.Location = TileLocation;
			InstigatorASC->ExecuteGameplayCue(SourceItem->SmashGCTag, GCParams);
		}
	}

	// 아이템을 활성화합니다.
	SourceItem->ActivateItem(Instigator);
}

ASMTile* USMCatchInteractionComponent_CatchableItem_AttributeChanger::TileTrigger(AActor* Instigator)
{
	if (!ensureAlways(Instigator))
	{
		return nullptr;
	}

	FHitResult HitResult;
	const FVector Start = Instigator->GetActorLocation();
	const FVector End = Start + (-FVector::UpVector * 1000.0f);
	const FCollisionQueryParams Param(SCENE_QUERY_STAT(TileTrace), false);
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(25.0f);
	const bool bSuccess = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, SMCollisionTraceChannel::TileAction, CollisionShape, Param);
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 3.0f);

	if (bSuccess)
	{
		ASMTile* TriggeredTile = Cast<ASMTile>(HitResult.GetActor());
		if (TriggeredTile)
		{
			return TriggeredTile;
		}
	}

	return nullptr;
}

void USMCatchInteractionComponent_CatchableItem_AttributeChanger::SaveTriggeredTilesBySmash(ASMTile* InTriggeredTile, float InMagnitude)
{
	// 타일의 가로 사이즈를 구합니다. 타일을 탑뷰 기준으로 정사각형이기 때문에 가로 사이즈만 구하면 세로 사이즈도 알 수 있습니다.
	UBoxComponent* TileBoxComponent = InTriggeredTile->GetBoxComponent();
	float TileHorizonSize = TileBoxComponent->GetScaledBoxExtent().X * 2;

	// 아이템이 적용되야할 타일의 위치를 구하기 위한 범위입니다.
	TArray<FOverlapResult> OverlapResults;
	const FVector Start = InTriggeredTile->GetTileLocation();
	float HalfHorizenSize = (TileHorizonSize * (InMagnitude - 1)) + 1.0f;

	FVector CollisionHalfExtend;
	if (HalfHorizenSize > 0.0f)
	{
		CollisionHalfExtend = FVector(HalfHorizenSize, HalfHorizenSize, 100.0);
	}

	const FCollisionShape CollisionShape = FCollisionShape::MakeBox(CollisionHalfExtend);
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(AttributeChanger), false);
	const bool bSuccess = GetWorld()->OverlapMultiByChannel(OverlapResults, Start, FQuat::Identity, SMCollisionTraceChannel::TileAction, CollisionShape, Params);

	if (bSuccess)
	{
		for (const auto& OverlapResult : OverlapResults)
		{
			ASMTile* TriggeredTile = Cast<ASMTile>(OverlapResult.GetActor());
			if (!ensureAlways(TriggeredTile))
			{
				continue;
			}

			ASMCatchableItem_AttributeChanger* SourceItem = Cast<ASMCatchableItem_AttributeChanger>(SourceActor);
			if (!ensureAlways(SourceItem))
			{
				continue;
			}

			// 트리거된 타일들을 아이템에게 저장해줍니다.
			SourceItem->TriggeredTiles.AddUnique(TriggeredTile);
		}
	}

	if (bDrawDebug)
	{
		DrawDebugBox(GetWorld(), Start, CollisionHalfExtend, FColor::Purple, false, 1.0f);
	}
}
