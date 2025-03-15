// Copyright Studio Surround. All Rights Reserved.


#include "SMGCN_Sampling.h"

#include "FMODBlueprintStatics.h"
#include "FunctionLibraries/SMGameplayCueBlueprintLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"

bool USMGCN_Sampling::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	if (!MyTarget)
	{
		return false;
	}

	FVector FXLocation;
	FRotator FXRotation;
	USMGameplayCueBlueprintLibrary::GetLocationAndRotation(Parameters, FXLocation, FXRotation);

	const ESMTeam OwnerTeam = USMTeamBlueprintLibrary::GetTeam(MyTarget);

	if (UFMODEvent* CachedSFX = SFX.Contains(OwnerTeam) ? SFX[OwnerTeam] : nullptr)
	{
		const FFMODEventInstance SFXInstance = UFMODBlueprintStatics::PlayEventAtLocation(MyTarget, CachedSFX, FTransform(FXLocation), true);

		const FName ParameterName = TEXT("SamplingStep");
		const float ParameterValue = Parameters.AbilityLevel;
		UFMODBlueprintStatics::EventInstanceSetParameter(SFXInstance, ParameterName, ParameterValue);
	}

	return true;
}
