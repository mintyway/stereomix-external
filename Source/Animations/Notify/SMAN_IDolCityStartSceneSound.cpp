// Copyright Studio Surround. All Rights Reserved.


#include "SMAN_IDolCityStartSceneSound.h"

#include "FMODBlueprintStatics.h"

void USMAN_IDolCityStartSceneSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	const UWorld* World = MeshComp ? MeshComp->GetWorld() : nullptr;
	if (!World)
	{
		return;
	}

	if (World->IsPreviewWorld()) // 미리보기용 사운드입니다.
	{
		UFMODBlueprintStatics::PlayEventAtLocation(MeshComp, Event, MeshComp->GetComponentTransform(), true);
	}
	else
	{
		const AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor && !OwnerActor->IsHidden()) // 숨겨지지 않은 경우만 사운드를 재생합니다.
		{
			UFMODBlueprintStatics::PlayEventAtLocation(MeshComp, Event, MeshComp->GetComponentTransform(), true);
		}
	}
}
