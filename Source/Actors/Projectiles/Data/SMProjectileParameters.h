#pragma once

#include "SMProjectileParameters.generated.h"

USTRUCT()
struct FSMProjectileParameters
{
	GENERATED_BODY()

	/** 투사체의 이용자입니다. */
	TWeakObjectPtr<AActor> Owner;

	/** 투사체의 생성지점입니다. */
	FVector StartLocation;

	/** 투사체가 나아갈 방향입니다. */
	FVector Normal;

	/** 투사체의 속도입니다. */
	float Speed = 1000.0f;

	/** 투사체의 최대 사거리입니다. 만약 음수라면 사실상 무한한 사거리를 갖습니다. (약 1km의 사거리를 가짐) */
	float MaxDistance = -1.0f;
};
