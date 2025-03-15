// Copyright Studio Surround. All Rights Reserved.


#include "SMAN_PlayFaceAnimation.h"

#include "Utilities/SMLog.h"

const FName USMAN_PlayFaceAnimation::FrameParameterName = TEXT("Frame");
const FName USMAN_PlayFaceAnimation::RowParameterName = (TEXT("RowCount"));
const FName USMAN_PlayFaceAnimation::ColumnParameterName = (TEXT("ColumnCount"));
const FName USMAN_PlayFaceAnimation::SpriteParameterName = (TEXT("Sprite"));
const FName USMAN_PlayFaceAnimation::ColorParameterName = (TEXT("Color"));


USMAN_PlayFaceAnimation::USMAN_PlayFaceAnimation()
{
	// NotifyColor = FColor(0, 128, 255);
}

FString USMAN_PlayFaceAnimation::GetNotifyName_Implementation() const
{
	return TEXT("PlayFaceAnimation");
}

void USMAN_PlayFaceAnimation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->IsNetMode(NM_DedicatedServer))
	{
		return;
	}

	const UWorld* World = MeshComp->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("World[%p] 혹은 Mesh[%p]가 유효하지 않아 Face Animation 재생에 실패했습니다. "), World, MeshComp);
		return;
	}
	if (!ParentMaterial || !ParentOverlayMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("부모 머티리얼이 존재하지 않습니다. 블루프린트에서 부모 머티리얼을 설정해주세요. "));
		return;
	}
	if (!SpriteData.IsValid() || !MaterialIndices.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Face Animation 재생에 필요한 데이터가 유효하지 않습니다. 스프라이트 데이터나 머티리얼 인덱스 정보를 확인해주세요. "));
		return;
	}

	// 만약 유효하지 않은 메시 컴포넌트의 TimerHandle이 있다면 이를 제거해줍니다.
	for (auto it = GlobalFaceAnimationTimerHandleMap.CreateIterator(); it; ++it)
	{
		if (!it->Key.IsValid())
		{
			World->GetTimerManager().ClearTimer(it->Value);
			it.RemoveCurrent();
		}
	}

	// 현재 메시에 연결된 타이머 핸들이 있다면 초기화해줍니다.
	if (FTimerHandle* ExistingTimerHandle = GlobalFaceAnimationTimerHandleMap.Find(MeshComp))
	{
		World->GetTimerManager().ClearTimer(*ExistingTimerHandle);
	}

	// MID를 생성하고 적용합니다.
	MIDs.Reset();
	if (DefaultMID = UMaterialInstanceDynamic::Create(ParentMaterial, MeshComp); DefaultMID)
	{
		MIDs.Add(DefaultMID);
		MeshComp->SetMaterial(MaterialIndices.MouseMaterialIndex, DefaultMID);
		MeshComp->SetMaterial(MaterialIndices.EyesMaterialIndex, DefaultMID);
	}
	if (OverlayMID = UMaterialInstanceDynamic::Create(ParentOverlayMaterial, MeshComp); OverlayMID)
	{
		OverlayMID->SetVectorParameterValue(ColorParameterName, OverlayColor);

		MIDs.Add(OverlayMID);
		MeshComp->SetMaterial(MaterialIndices.EyesOverlayMaterialIndex, OverlayMID);
	}

	// 기본 파라미터들을 초기화합니다.
	for (UMaterialInstanceDynamic* MID : MIDs)
	{
		MID->SetScalarParameterValue(FrameParameterName, 0.0f);
		MID->SetScalarParameterValue(RowParameterName, SpriteData.Row);
		MID->SetScalarParameterValue(ColumnParameterName, SpriteData.Column);
		MID->SetTextureParameterValue(SpriteParameterName, SpriteData.SpriteImage);
	}

	// 해당 메시에 연결할 새로운 타이머 핸들을 생성하고 추가합니다.
	FTimerHandle NewTimerHandle;
	GlobalFaceAnimationTimerHandleMap.Add(MeshComp, NewTimerHandle);
	World->GetTimerManager().SetTimer(NewTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::OnUpdate, MakeWeakObjectPtr(World), MakeWeakObjectPtr(MeshComp), 1, SpriteData.GetFrameCount()), FrameRate.AsInterval(), false);
}

void USMAN_PlayFaceAnimation::OnUpdate(TWeakObjectPtr<const UWorld> World, TWeakObjectPtr<USkeletalMeshComponent> MeshComp, int32 CurrentFrame, int32 TotalFrameCount)
{
	if (!World.IsValid() || CurrentFrame >= TotalFrameCount)
	{
		return;
	}

	for (UMaterialInstanceDynamic* MID : MIDs)
	{
		if (MID)
		{
			MID->SetScalarParameterValue(FrameParameterName, CurrentFrame);
		}
	}

	if (FTimerHandle* ExistingTimerHandle = GlobalFaceAnimationTimerHandleMap.Find(MeshComp))
	{
		World->GetTimerManager().SetTimer(*ExistingTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::OnUpdate, World, MeshComp, CurrentFrame + 1, TotalFrameCount), FrameRate.AsInterval(), false);
	}
}
