#include "StereoMixLog.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY(LogStereoMix)
DEFINE_LOG_CATEGORY(LogStereoMixUI)
DEFINE_LOG_CATEGORY(LogStereoMixFlow)
DEFINE_LOG_CATEGORY(LogStereoMixTeams)
DEFINE_LOG_CATEGORY(LogStereoMixOnline)
DEFINE_LOG_CATEGORY(LogStereoMixAbilitySystem);

FString GetClientServerContextString(UObject* ContextObject)
{
	ENetRole Role = ROLE_None;

	if (AActor* Actor = Cast<AActor>(ContextObject))
	{
		Role = Actor->GetLocalRole();
	}
	else if (UActorComponent* Component = Cast<UActorComponent>(ContextObject))
	{
		Role = Component->GetOwnerRole();
	}

	if (Role != ROLE_None)
	{
		return (Role == ROLE_Authority) ? TEXT("Server") : TEXT("Client");
	}
	else
	{
#if WITH_EDITOR
		if (GIsEditor)
		{
			extern ENGINE_API FString GPlayInEditorContextString;
			return GPlayInEditorContextString;
		}
#endif
	}

	return TEXT("[Unknown]");
}
