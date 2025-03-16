// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacterBase.h"
#include "SMElectricGuitarCharacter.generated.h"

UCLASS()
class STEREOMIX_API ASMElectricGuitarCharacter : public ASMPlayerCharacterBase
{
	GENERATED_BODY()

public:
	ASMElectricGuitarCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void OnHoldStateEntry() override;

	virtual void OnHoldStateExit() override;
};
