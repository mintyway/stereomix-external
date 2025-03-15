// Copyright Studio Surround. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SMMatchmakingSubsystem.generated.h"

class USMMatchmakingSubsystem;
class USMMatchmakingTicket;

UENUM()
enum class ESMMatchmakingType
{
	Matchmaking,
	CustomMatch
};

UENUM(BlueprintType)
enum class ESMMatchmakingTicketStatus : uint8
{
	Pending,
	InProgress,
	Completed,
	Failed,
	Cancelled
};

UENUM(BlueprintType)
enum class ESMMatchmakingRegions : uint8
{
	All,
	Asia,
	Europe,
	NorthAmerica,
	SouthAmerica,
	Africa,
	Oceania
};

UENUM(BlueprintType)
enum class ESMMatchmakingCreateFailedReason : uint8
{
	Unknown,
	InvalidVersion,
	InvalidRegion,
	InvalidMatchType,
	InvalidName,
	InvalidPassword,
	InternalError
};

USTRUCT(BlueprintType)
struct FSMMatchmakingTicketResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString TicketId;

	UPROPERTY(BlueprintReadOnly)
	ESMMatchmakingTicketStatus Status{};

	UPROPERTY(BlueprintReadOnly)
	FString Link;

	UPROPERTY(BlueprintReadOnly)
	FDateTime CreatedAt;
};

USTRUCT()
struct FSMCustomMatchRequest
{
	GENERATED_BODY()

	UPROPERTY()
	FString Version;

	UPROPERTY()
	ESMMatchmakingRegions Region{};

	UPROPERTY()
	FString MatchType;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	bool IsPrivate{};

	UPROPERTY()
	FString Password;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSMOnMatchmakingResponse, bool, bWasSuccessful, const USMMatchmakingTicket*, Ticket,  const FText&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSMOnMatchmakingTicketUpdated, const USMMatchmakingTicket*, Ticket);

UCLASS(BlueprintType)
class STEREOMIX_API USMMatchmakingTicket : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Matchmaking")
	void UpdateTicket();
	
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Matchmaking")
	void CancelTicket();

	void ResetTicket();
	
	UPROPERTY(BlueprintReadOnly)
	ESMMatchmakingType MatchmakingType;

	UPROPERTY(BlueprintReadOnly)
	FString TicketId;

	UPROPERTY(BlueprintReadOnly)
	ESMMatchmakingTicketStatus Status;

	UPROPERTY(BlueprintReadOnly)
	FString Link;

	UPROPERTY(BlueprintReadOnly)
	double CreatedAt;

	UPROPERTY(BlueprintReadOnly)
	double UpdatedAt;

	UPROPERTY(BlueprintAssignable, Category = "StereoMix|Matchmaking")
	FSMOnMatchmakingTicketUpdated OnTicketUpdatedEvent;

protected:
	TSharedPtr<IHttpRequest> CreateHttpRequest(const FString& InVerb) const;
	
	void HandleTicketStatusResponse(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bWasSuccessful);
	void HandleCancelTicketResponse(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bWasSuccessful);
	
	TObjectPtr<USMMatchmakingSubsystem> Matchmaker;

private:
	TSharedPtr<IHttpRequest> ActiveRequest;
};


UCLASS(BlueprintType, Blueprintable)
class USMMatchmakingRequest : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "StereoMix|Matchmaking")
	ESMMatchmakingRegions Region;
	
	UPROPERTY(BlueprintReadWrite, Category = "StereoMix|Matchmaking")
	FString MatchType;
	
	UPROPERTY(BlueprintAssignable, Category = "StereoMix|Matchmaking")
	FSMOnMatchmakingResponse OnResponseEvent;

	virtual bool IsValidRequest() const;
	void CancelRequest();

protected:
	TSharedPtr<IHttpRequest> HttpRequest;

	friend class USMMatchmakingSubsystem;
};

UCLASS()
class USMCustomMatchCreateRequest : public USMMatchmakingRequest
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "StereoMix|Matchmaking")
	FString Name;

	UPROPERTY(BlueprintReadWrite, Category = "StereoMix|Matchmaking")
	bool bIsPrivate;

	UPROPERTY(BlueprintReadWrite, Category = "StereoMix|Matchmaking")
	FString Password;
	
	virtual bool IsValidRequest() const override;
};

UCLASS(DisplayName = "StereoMix Matchmaking")
class STEREOMIX_API USMMatchmakingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static USMMatchmakingSubsystem& Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Matchmaking")
	USMMatchmakingTicket* GetActiveMatchmakingTicket() const { return ActiveMatchmakingTicket.Get(); }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Matchmaking")
	bool IsMatchmaking() const { return ActiveMatchmakingTicket.IsValid(); }

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Matchmaking")
	void StartMatchmaking();

	UFUNCTION(BlueprintCallable, Category = "StereoMix|Matchmaking")
	USMCustomMatchCreateRequest* CreateCustomMatchRequest(FString MatchType, FString Name, bool bIsPrivate, FString Password);
	
	UFUNCTION(BlueprintCallable, Category = "StereoMix|Matchmaking")
	bool CreateCustomMatch(USMCustomMatchCreateRequest* Request);

	UPROPERTY(BlueprintAssignable)
	FSMOnMatchmakingTicketUpdated OnMatchmakingTicketUpdatedEvent;

protected:
	struct FSMMatchmakingHttpRequest
	{
		ESMMatchmakingType MatchmakingType;
		TSharedPtr<IHttpRequest> Request;
	};

	static FString GetCustomMatchUrl();
	void ResetActiveRequest();

	void StartTicketPolling();
	void StopTicketPolling();
	void PollingTicket();

	UFUNCTION()
	void HandleMatchmakingTicketUpdated(const USMMatchmakingTicket* Ticket);

	void HandleCreateCustomMatchResponse(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bWasSuccessful);

private:
	ESMMatchmakingType ActiveMatchmakingType;

	FTimerHandle MatchmakingTicketPollingHandle;

	UPROPERTY()
	TWeakObjectPtr<USMMatchmakingRequest> ActiveRequest;
	
	UPROPERTY()
	TWeakObjectPtr<USMMatchmakingTicket> ActiveMatchmakingTicket;
};
