// Copyright Surround, Inc. All Rights Reserved.


#include "SMAT_CheckUnderTile.h"

#include "Actors/Character/SMCharacterBase.h"
#include "Actors/Tiles/SMTile.h"
#include "Components/CapsuleComponent.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"


USMAT_CheckUnderTile::USMAT_CheckUnderTile()
{
	bTickingTask = true;
}

USMAT_CheckUnderTile* USMAT_CheckUnderTile::CheckUnderTile(UGameplayAbility* OwningAbility, float TickRate)
{
	USMAT_CheckUnderTile* NewObj = NewAbilityTask<USMAT_CheckUnderTile>(OwningAbility);
	NewObj->OwnerCharacter = OwningAbility ? Cast<ASMCharacterBase>(OwningAbility->GetAvatarActorFromActorInfo()) : nullptr;
	NewObj->TickInterval = 1.0f / TickRate;
	return NewObj;
}

void USMAT_CheckUnderTile::Activate()
{
	Super::Activate();

	const UCapsuleComponent* OwnerCapsule = OwnerCharacter.IsValid() ? OwnerCharacter->GetCapsuleComponent() : nullptr;
	TileDetectionOffset = OwnerCapsule ? OwnerCapsule->GetScaledCapsuleHalfHeight() + 5.0f : 0.0f;
}

void USMAT_CheckUnderTile::TickTask(float DeltaTime)
{
	AccumulatedTime += DeltaTime;
	if (AccumulatedTime < TickInterval)
	{
		return;
	}

	AccumulatedTime -= TickInterval;

	if (!OwnerCharacter.Get())
	{
		return;
	}

	FHitResult HitResult;
	const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	const FVector StartLocation(OwnerLocation.X, OwnerLocation.Y, OwnerLocation.Z - TileDetectionOffset);
	const FVector EndLocation = StartLocation + (FVector::DownVector * 1000.0f);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, SMCollisionTraceChannel::TileAction))
	{
		ASMTile* Tile = Cast<ASMTile>(HitResult.GetActor());
		if (!Tile)
		{
			return;
		}

		if (LastCheckedTile.Get() == Tile) // 다른 팀 캐릭터 둘이 한 타일 위에 있는 경우 번갈아가며 타일을 체크하게되는데 이를 방지합니다.
		{
			const ESMTeam LastCapturedTileTeam = LastCheckedTile->GetTeam();
			if (LastCapturedTileTeam != ESMTeam::None) // 이는 튜토리얼을 위한 코드인데, 팀이 None인 경우는 이전에 체크했던 타일이더라도 재시도합니다.
			{
				return;
			}
		}

		LastCheckedTile = Tile;

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			(void)OnUnderTileChanged.ExecuteIfBound(Tile);
		}
	}
}

void USMAT_CheckUnderTile::OnDestroy(bool bInOwnerFinished)
{
	OnUnderTileChanged.Unbind();

	Super::OnDestroy(bInOwnerFinished);
}
