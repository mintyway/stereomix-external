// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/Enum/SMOutlineStencil.h"
#include "SMCharacterBase.generated.h"

class ASMWeaponBase;

UCLASS(Abstract)
class STEREOMIX_API ASMCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ASMCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_Controller() override;

	virtual void OnRep_PlayerState() override;

	ESMShaderStencil GetDefaultShaderStencil() const { return DefaultShaderStencil; }

	ASMWeaponBase* GetWeapon() const { return Weapon; }

	template<typename T>
	T* GetWeapon() const { return Cast<T>(GetWeapon()); }

	/** 스텐실 기본값을 다시 계산합니다. */
	virtual void RecalculateDefaultStencil();

	/** 스텐실을 기본값으로 초기화합니다. */
	virtual void ApplyDefaultStencil();

	/** 빙의되는 시점입니다. 서버라면 PossessedBy, 클라이언트라면 OnRep_Controller에서 호출됩니다. */
	FSimpleMulticastDelegate OnPossessed;
	
protected:
	UFUNCTION()
	void OnRep_Weapon();

	ESMShaderStencil DefaultShaderStencil = ESMShaderStencil::NonOutline;

	UPROPERTY(ReplicatedUsing = "OnRep_Weapon")
	TObjectPtr<ASMWeaponBase> Weapon;
};
