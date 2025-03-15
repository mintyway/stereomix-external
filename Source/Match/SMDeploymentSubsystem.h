// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SMDeploymentSubsystem.generated.h"

class IHttpRequest;
class IHttpResponse;

struct FSMDeploymentLocation
{
	FString City;
	FString Country;
	FString Continent;
	FString AdministrativeDivision;
	FString Timezone;
	float Latitude;
	float Longitude;
};

struct FSMDeploymentPort
{
	FString Protocol;
	int32 Internal;
	int32 External;
};

// struct FSMDeploymentContext
// {
// 	FString RequestId;
// 	FString PublicIp;
// 	FSMDeploymentLocation Location;
// 	FSMDeploymentPort Port;
// };

/**
 * Edgegap을 통해 생성된 게임서버가 사용하는 배포 관련 서브시스템입니다.
 */
UCLASS(DisplayName = "StereoMix Deployment Subsystem")
class STEREOMIX_API USMDeploymentSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static USMDeploymentSubsystem* Get(const UObject* WorldContextObject);
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	bool IsDeployment() const { return bIsDeployment; }
	FString GetRequestId() const { return RequestId; }
	FString GetPublicIP() const { return PublicIP; }
	FSMDeploymentLocation GetDeploymentLocation() const { return Location; }
	FSMDeploymentPort GetPort() const { return Port; }
	
	// void GetContext();
	void Delete();

protected:
	void InitializeDeployment();
	void DumpDeploymentInfo() const;
	
	// void HandleGetContextResponse(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful);
	void HandleDeleteResponse(TSharedPtr<IHttpRequest> Request, TSharedPtr<IHttpResponse> Response, bool bWasSuccessful);
	
private:
	bool bIsDeployment;

	FString RequestId;
	FString DeleteUrl;
	FString DeleteToken;
	FString ContextUrl;
	FString ContextToken;
	FString PublicIP;
	FSMDeploymentLocation Location;
	FSMDeploymentPort Port;
};
