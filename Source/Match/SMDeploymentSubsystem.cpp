// Copyright Studio Surround. All Rights Reserved.


#include "SMDeploymentSubsystem.h"

#include "HttpModule.h"
#include "SMMatchLog.h"
#include "Interfaces/IHttpResponse.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMDeploymentSubsystem)

#define DEPLOYMENT_ENV_REQUEST_ID TEXT("ARBITRIUM_REQUEST_ID")
#define DEPLOYMENT_ENV_DELETE_URL TEXT("ARBITRIUM_DELETE_URL")
#define DEPLOYMENT_ENV_DELETE_TOKEN TEXT("ARBITRIUM_DELETE_TOKEN")
#define DEPLOYMENT_ENV_LOCATION TEXT("ARBITRIUM_DEPLOYMENT_LOCATION")
#define DEPLOYMENT_ENV_CONTEXT_URL TEXT("ARBITRIUM_CONTEXT_URL")
#define DEPLOYMENT_ENV_CONTEXT_TOKEN TEXT("ARBITRIUM_CONTEXT_TOKEN")
#define DEPLOYMENT_ENV_PUBLIC_IP TEXT("ARBITRIUM_PUBLIC_IP")
#define DEPLOYMENT_ENV_PORTS_MAPPING TEXT("ARBITRIUM_PORTS_MAPPING")
#define DEPLOYMENT_ENV_PORT_INTERNAL TEXT("ARBITRIUM_PORT_GAME_INTERNAL")
#define DEPLOYMENT_ENV_PORT_EXTERNAL TEXT("ARBITRIUM_PORT_GAME_EXTERNAL")
#define DEPLOYMENT_ENV_PORT_PROTOCOL TEXT("ARBITRIUM_PORT_GAME_PROTOCOL")

USMDeploymentSubsystem* USMDeploymentSubsystem::Get(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return World->GetGameInstance()->GetSubsystem<USMDeploymentSubsystem>();
	}
	return nullptr;
}

bool USMDeploymentSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// Deployment는 게임서버만 사용가능함
	const UGameInstance* GameInstance = CastChecked<UGameInstance>(Outer);
	return GameInstance->IsDedicatedServerInstance();
}

void USMDeploymentSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (GetWorld())
	{
		if (FParse::Param(FCommandLine::Get(), TEXT("Deployment")))
		{
			UE_LOG(LogStereoMixDeployment, Log, TEXT("Server deployment is enabled."));
			bIsDeployment = true;
			InitializeDeployment();
			DumpDeploymentInfo();
		}
		else
		{
			UE_LOG(LogStereoMixDeployment, Log, TEXT("Server deployment is disabled."));
			bIsDeployment = false;
		}
	}
}

void USMDeploymentSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void USMDeploymentSubsystem::Delete()
{
	FHttpModule& HttpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = HttpModule.CreateRequest();
	Request->SetURL(DeleteUrl);
	Request->SetVerb(TEXT("DELETE"));
	Request->SetHeader(TEXT("Authorization"), DeleteToken);
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::HandleDeleteResponse);
	Request->ProcessRequest();
}

void USMDeploymentSubsystem::InitializeDeployment()
{
	RequestId = FPlatformMisc::GetEnvironmentVariable(DEPLOYMENT_ENV_REQUEST_ID);
	DeleteUrl = FPlatformMisc::GetEnvironmentVariable(DEPLOYMENT_ENV_DELETE_URL);
	DeleteToken = FPlatformMisc::GetEnvironmentVariable(DEPLOYMENT_ENV_DELETE_TOKEN);
	ContextUrl = FPlatformMisc::GetEnvironmentVariable(DEPLOYMENT_ENV_CONTEXT_URL);
	ContextToken = FPlatformMisc::GetEnvironmentVariable(DEPLOYMENT_ENV_CONTEXT_TOKEN);
	PublicIP = FPlatformMisc::GetEnvironmentVariable(DEPLOYMENT_ENV_PUBLIC_IP);

	FString LocationJson = FPlatformMisc::GetEnvironmentVariable(DEPLOYMENT_ENV_LOCATION);
	if (!LocationJson.IsEmpty())
	{
		TSharedPtr<FJsonObject> LocationObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(LocationJson);
		if (FJsonSerializer::Deserialize(Reader, LocationObject))
		{
			Location.City = LocationObject->GetStringField(TEXT("city"));
			Location.Country = LocationObject->GetStringField(TEXT("country"));
			Location.Continent = LocationObject->GetStringField(TEXT("continent"));
			Location.AdministrativeDivision = LocationObject->GetStringField(TEXT("administrative_division"));
			Location.Timezone = LocationObject->GetStringField(TEXT("timezone"));
			Location.Latitude = LocationObject->GetNumberField(TEXT("latitude"));
			Location.Longitude = LocationObject->GetNumberField(TEXT("longitude"));
		}
	}

	Port.Protocol = FPlatformMisc::GetEnvironmentVariable(DEPLOYMENT_ENV_PORT_PROTOCOL);
	Port.Internal = FCString::Atoi(*FPlatformMisc::GetEnvironmentVariable(DEPLOYMENT_ENV_PORT_INTERNAL));
	Port.External = FCString::Atoi(*FPlatformMisc::GetEnvironmentVariable(DEPLOYMENT_ENV_PORT_EXTERNAL));
}

void USMDeploymentSubsystem::DumpDeploymentInfo() const
{
	UE_LOG(LogStereoMixDeployment, Log, TEXT("[Deployment Info]"));
	UE_LOG(LogStereoMixDeployment, Log, TEXT("RequestId: %s"), *RequestId);
	UE_LOG(LogStereoMixDeployment, Log, TEXT("DeleteUrl: %s"), *DeleteUrl);
	UE_LOG(LogStereoMixDeployment, Log, TEXT("DeleteToken: %s"), *DeleteToken);
	UE_LOG(LogStereoMixDeployment, Log, TEXT("ContextUrl: %s"), *ContextUrl);
	UE_LOG(LogStereoMixDeployment, Log, TEXT("ContextToken: %s"), *ContextToken);
	UE_LOG(LogStereoMixDeployment, Log, TEXT("PublicIp: %s"), *PublicIP);
	UE_LOG(LogStereoMixDeployment, Log, TEXT("Location: %s, %s, %s, %s, %s, %f, %f"), *Location.City, *Location.Country, *Location.Continent, *Location.AdministrativeDivision, *Location.Timezone, Location.Latitude, Location.Longitude);
	UE_LOG(LogStereoMixDeployment, Log, TEXT("Port: %s, %d, %d"), *Port.Protocol, Port.Internal, Port.External);
}

void USMDeploymentSubsystem::HandleDeleteResponse(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		int32 ResponseCode = Response->GetResponseCode();
		if (ResponseCode == EHttpResponseCodes::Ok || ResponseCode == EHttpResponseCodes::Accepted)
		{
			UE_LOG(LogStereoMixDeployment, Log, TEXT("Server deployment will be deleted."));
			return;
		}
	}

	UE_LOG(LogStereoMixDeployment, Error, TEXT("Failed to delete server deployment."));
}
