// Copyright Surround, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODBlueprintStatics.h"
#include "SMFMODBlueprintStatics.generated.h"

/**
 * 
 */
UCLASS()
class STEREOMIX_API USMFMODBlueprintStatics : public UFMODBlueprintStatics
{
	GENERATED_BODY()

public:
	/** 해당 Bus의 볼륨 값을 반환합니다. 실패시 0을 반환합니다.*/
	UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|Bus", meta = (UnsafeDuringActorConstruction))
	static float BusGetVolume(UFMODBus* Bus);

	/** 해당 VCA의 볼륨 값을 반환합니다. 실패시 0을 반환합니다.*/
	UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|VCA", meta = (UnsafeDuringActorConstruction))
	static float VCAGetVolume(UFMODVCA* VCA);

	/** 현재 사용중인 드라이버의 이름을 반환합니다. 실패시 빈 문자열을 반환합니다.*/
	UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|VCA", meta = (UnsafeDuringActorConstruction))
	static FString GetCurrentDriverName();

	/** 현재 사용중인 드라이버의 인덱스를 반환합니다. 실패시 0을 반환합니다.*/
	UFUNCTION(BlueprintCallable, Category = "Audio|FMOD|VCA", meta = (UnsafeDuringActorConstruction))
	static int32 GetCurrentDriverIndex();
};
