// Copyright Studio Surround. All Rights Reserved.


#include "SMMatchmakingSubsystem.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonObjectConverter.h"
#include "SMMatchLog.h"
#include "StereoMixSDKModule.h"
#include "Internationalization/StringTableRegistry.h"
#include "Online/SMUserSubsystem.h"
#include "System/SMGameInstance.h"
#include "System/SMGameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SMMatchmakingSubsystem)

#define LOCTABLE_ID "Game"

FString MatchmakingPath = TEXT("queue");
FString CustomMatchPath = TEXT("custom");

// 쓰레기 코드

void USMMatchmakingTicket::UpdateTicket()
{
	if (ActiveRequest.IsValid())
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("Other matchmaking request is already in progress."));
		return;
	}

	TSharedPtr<IHttpRequest> Request = CreateHttpRequest(TEXT("GET"));
	if (!Request)
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("UpdateTicket: Failed to create HTTP request."));
		return;
	}
	UE_LOG(LogStereoMixMatchmaking, Verbose, TEXT("UpdateTicket: %s"), *Request->GetURL());
	Request->ProcessRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::HandleTicketStatusResponse);
	ActiveRequest = Request;
}

void USMMatchmakingTicket::CancelTicket()
{
	if (ActiveRequest.IsValid())
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("Other matchmaking request is already in progress."));
		return;
	}

	TSharedPtr<IHttpRequest> Request = CreateHttpRequest(TEXT("DELETE"));
	if (!Request)
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("CancelTicket: Failed to create HTTP request."));
		return;
	}
	UE_LOG(LogStereoMixMatchmaking, Verbose, TEXT("CancelTicket: %s"), *Request->GetURL());
	Request->ProcessRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::HandleCancelTicketResponse);
	ActiveRequest = Request;

	// API 응답에 상관없이 바로 제거
	// ResetTicket();
}

void USMMatchmakingTicket::ResetTicket()
{
	if (ActiveRequest.IsValid())
	{
		ActiveRequest->CancelRequest();
		ActiveRequest.Reset();

		Status = ESMMatchmakingTicketStatus::Cancelled;
		OnTicketUpdatedEvent.Broadcast(this);
	}
}

TSharedPtr<IHttpRequest> USMMatchmakingTicket::CreateHttpRequest(const FString& InVerb) const
{
	if (!GetWorld())
	{
		return nullptr;
	}

	USMUserSubsystem* UserSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USMUserSubsystem>();
	if (!UserSubsystem)
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("User subsystem not found."));
		return nullptr;
	}

	const USMUserInfo* UserInfo = UserSubsystem->GetStereoMixUserInfoForLocalPlayerIndex(0);
	if (!UserInfo)
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("User info not found."));
		return nullptr;
	}

	if (!UserInfo->IsLoggedIn())
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("User not logged in."));
		return nullptr;
	}

	FString UserToken = UserInfo->GetToken();
	if (UserToken.IsEmpty())
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("User token not found."));
		return nullptr;
	}

	if (TicketId.IsEmpty())
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("Ticket ID is empty."));
		return nullptr;
	}

	FStereoMixSDKModule& SDK = FStereoMixSDKModule::Get();
	FString Endpoint = SDK.GetServerEndpoint();

	FHttpModule& HttpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = HttpModule.CreateRequest();
	Request->SetURL(FString::Printf(TEXT("%s/match/%s/%s"), *Endpoint, *(MatchmakingType == ESMMatchmakingType::Matchmaking ? MatchmakingPath : CustomMatchPath), *TicketId));
	Request->SetVerb(InVerb);
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), "Bearer " + UserToken);
	return Request;
}

void USMMatchmakingTicket::HandleTicketStatusResponse(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bWasSuccessful)
{
	if (!GetWorld())
	{
		return;
	}

	UpdatedAt = GetWorld()->GetTimeSeconds();

	if (bWasSuccessful)
	{
		int32 ResponseCode = InResponse->GetResponseCode();
		if (ResponseCode == EHttpResponseCodes::Ok)
		{
			FString ResponseJson = InResponse->GetContentAsString();
			FSMMatchmakingTicketResponse Response;
			if (FJsonObjectConverter::JsonObjectStringToUStruct(ResponseJson, &Response, 0, 0))
			{
				Status = Response.Status;
				Link = Response.Link;
				UpdatedAt = GetWorld()->GetTimeSeconds();
				OnTicketUpdatedEvent.Broadcast(this);
				ActiveRequest.Reset();
				return;
			}
			UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleTicketStatusResponse: Failed to parse response JSON."));
		}
		else if (ResponseCode == EHttpResponseCodes::Denied)
		{
			UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleTicketStatusResponse: User unauthorized."));
		}
		else if (ResponseCode == EHttpResponseCodes::BadRequest)
		{
			UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleTicketStatusResponse: Invalid ticket ID."));
		}
		else if (ResponseCode == EHttpResponseCodes::ServerError)
		{
			UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleTicketStatusResponse: Internal server error."));
		}
		else
		{
			UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleTicketStatusResponse: Failed to get ticket status with HTTP code: %d"), ResponseCode);
		}
	}
	else
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleTicketStatusResponse: Failed to get ticket status: %d"), InResponse->GetStatus());
	}

	Status = ESMMatchmakingTicketStatus::Failed;
	OnTicketUpdatedEvent.Broadcast(this);
	ActiveRequest.Reset();
}

void USMMatchmakingTicket::HandleCancelTicketResponse(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bWasSuccessful)
{
	if (!GetWorld())
	{
		return;
	}

	UpdatedAt = GetWorld()->GetTimeSeconds();

	if (bWasSuccessful)
	{
		int32 ResponseCode = InResponse->GetResponseCode();
		if (ResponseCode == EHttpResponseCodes::Ok)
		{
			Status = ESMMatchmakingTicketStatus::Cancelled;
			OnTicketUpdatedEvent.Broadcast(this);
			ActiveRequest.Reset();
			return;
		}

		if (ResponseCode == EHttpResponseCodes::Denied)
		{
			UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleCancelTicketResponse: User unauthorized."));
		}
		else if (ResponseCode == EHttpResponseCodes::BadRequest)
		{
			UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleCancelTicketResponse: Invalid ticket ID."));
		}
		else if (ResponseCode == EHttpResponseCodes::ServerError)
		{
			UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleCancelTicketResponse: Internal server error."));
		}
		else
		{
			UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleCancelTicketResponse: Failed to cancel ticket with HTTP code: %d"), ResponseCode);
		}
	}
	else
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleCancelTicketResponse: Failed to cancel ticket"));
	}

	Status = ESMMatchmakingTicketStatus::Failed;
	OnTicketUpdatedEvent.Broadcast(this);
	ActiveRequest.Reset();
}

bool USMCustomMatchCreateRequest::IsValidRequest() const
{
	if (!Super::IsValidRequest())
	{
		return false;
	}

	if (Name.IsEmpty())
	{
		return false;
	}
	if (bIsPrivate && Password.IsEmpty())
	{
		return false;
	}
	return true;
}

bool USMMatchmakingRequest::IsValidRequest() const
{
	if (MatchType.IsEmpty())
	{
		return false;
	}
	return true;
}

void USMMatchmakingRequest::CancelRequest()
{
	if (HttpRequest.IsValid())
	{
		HttpRequest->CancelRequest();
		HttpRequest.Reset();
	}
}

USMMatchmakingSubsystem& USMMatchmakingSubsystem::Get(const UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
	check(World);
	USMMatchmakingSubsystem* SubSystem = World->GetSubsystem<USMMatchmakingSubsystem>();
	check(SubSystem);
	return *SubSystem;
}

void USMMatchmakingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USMMatchmakingSubsystem::Deinitialize()
{
	Super::Deinitialize();
	ResetActiveRequest();

	if (ActiveRequest.IsValid())
	{
		UE_LOG(LogStereoMixMatchmaking, Warning, TEXT("Active matchmaking request is in progress. Request cancelled."));
		ActiveRequest->CancelRequest();
		ActiveRequest.Reset();
	}

	if (ActiveMatchmakingTicket.IsValid())
	{
		UE_LOG(LogStereoMixMatchmaking, Warning, TEXT("Active matchmaking ticket is in progress. Ticket cancelled."));
		StopTicketPolling();
		ActiveMatchmakingTicket->CancelTicket();
		ActiveMatchmakingTicket->ResetTicket();
	}
}

bool USMMatchmakingSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void USMMatchmakingSubsystem::StartMatchmaking()
{
	ensure(true); // NOT IMPLEMENTED
}

USMCustomMatchCreateRequest* USMMatchmakingSubsystem::CreateCustomMatchRequest(FString MatchType, FString Name, bool bIsPrivate, FString Password)
{
	USMCustomMatchCreateRequest* Request = NewObject<USMCustomMatchCreateRequest>();
	Request->Region = ESMMatchmakingRegions::All; // TODO: 설정에서 가져오기
	Request->MatchType = MatchType;
	Request->Name = Name;
	Request->bIsPrivate = bIsPrivate;
	Request->Password = Password;
	return Request;
}

bool USMMatchmakingSubsystem::CreateCustomMatch(USMCustomMatchCreateRequest* Request)
{
	if (!Request || !Request->IsValidRequest())
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("Request is invalid."));
		return false;
	}

	if (ActiveRequest.IsValid())
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("Other matchmaking request is already in progress."));
		return false;
	}

	USMUserSubsystem* UserSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USMUserSubsystem>();
	if (!UserSubsystem)
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("User subsystem not found."));
		return false;
	}

	const USMUserInfo* UserInfo = UserSubsystem->GetStereoMixUserInfoForLocalPlayerIndex(0);
	if (!UserInfo)
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("User info not found."));
		return false;
	}

	if (!UserInfo->IsLoggedIn())
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("User not logged in."));
		return false;
	}

	FString UserToken = UserInfo->GetToken();
	if (UserToken.IsEmpty())
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("User token not found."));
		return false;
	}

	FStereoMixSDKModule& SDK = FStereoMixSDKModule::Get();
	FString Endpoint = SDK.GetServerEndpoint();

	FHttpModule& HttpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> HttpRequest = HttpModule.CreateRequest();

	FSMCustomMatchRequest RequestBody;
	RequestBody.Version = USMGameplayStatics::GetProjectVersion();
	RequestBody.Region = Request->Region;
	RequestBody.MatchType = Request->MatchType;
	RequestBody.Name = Request->Name;
	RequestBody.IsPrivate = Request->bIsPrivate;
	RequestBody.Password = Request->Password;

	FString RequestJson;
	FJsonObjectConverter::UStructToJsonObjectString(RequestBody, RequestJson);

	HttpRequest->SetURL(Endpoint + "/match/custom");
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Authorization"), "Bearer " + UserToken);
	HttpRequest->SetContentAsString(RequestJson);
	UE_LOG(LogStereoMixMatchmaking, Verbose, TEXT("CreateCustomMatch: %s"), *HttpRequest->GetURL());
	HttpRequest->ProcessRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &ThisClass::HandleCreateCustomMatchResponse);

	ActiveMatchmakingType = ESMMatchmakingType::CustomMatch;
	Request->HttpRequest = HttpRequest;
	ActiveRequest = Request;
	return true;
}

FString USMMatchmakingSubsystem::GetCustomMatchUrl()
{
	FString Endpoint = FStereoMixSDKModule::Get().GetServerEndpoint();
	return FString::Printf(TEXT("%s%s%s"), *Endpoint, *MatchmakingPath, *CustomMatchPath);
}

void USMMatchmakingSubsystem::ResetActiveRequest()
{
	if (ActiveRequest.IsValid())
	{
		ActiveRequest->CancelRequest();
		ActiveRequest.Reset();
	}
}

void USMMatchmakingSubsystem::StartTicketPolling()
{
	if (MatchmakingTicketPollingHandle.IsValid())
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("Ticket polling is already in progress."));
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(MatchmakingTicketPollingHandle, this, &ThisClass::PollingTicket, 1.0f, true);
}

void USMMatchmakingSubsystem::StopTicketPolling()
{
	if (!MatchmakingTicketPollingHandle.IsValid())
	{
		UE_LOG(LogStereoMixMatchmaking, Warning, TEXT("Ticket polling is not in progress."));
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(MatchmakingTicketPollingHandle);
}

void USMMatchmakingSubsystem::PollingTicket()
{
	if (!ActiveMatchmakingTicket.IsValid())
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("Active matchmaking ticket is not valid."));
		StopTicketPolling();
		return;
	}

	ActiveMatchmakingTicket->UpdateTicket();
}

void USMMatchmakingSubsystem::HandleMatchmakingTicketUpdated(const USMMatchmakingTicket* Ticket)
{
	if (Ticket != ActiveMatchmakingTicket.Get())
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleMatchmakingTicketUpdated: Ticket mismatch."));
		return;
	}

	if (Ticket->Status == ESMMatchmakingTicketStatus::Completed)
	{
		UE_LOG(LogStereoMixMatchmaking, Log, TEXT("Matchmaking ticket completed. stop polling."));
		ActiveMatchmakingTicket.Reset();
		StopTicketPolling();
	}
	else if (Ticket->Status == ESMMatchmakingTicketStatus::Cancelled || Ticket->Status == ESMMatchmakingTicketStatus::Failed)
	{
		if (Ticket->Status == ESMMatchmakingTicketStatus::Cancelled)
		{
			if (ActiveRequest.IsValid())
			{
				UE_LOG(LogStereoMixMatchmaking, Log, TEXT("Matchmaking ticket cancelled and current request is in progress. cancel active polling request."));
				ActiveRequest->CancelRequest();
			}
		}

		UE_LOG(LogStereoMixMatchmaking, Log, TEXT("Matchmaking ticket cancelled or failed. stop polling."));
		ActiveMatchmakingTicket.Reset();
		StopTicketPolling();
	}
}

void USMMatchmakingSubsystem::HandleCreateCustomMatchResponse(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bWasSuccessful)
{
	if (!ActiveRequest.IsValid())
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("No active request to handle response."));
		return;
	}

	if (!ensure(InRequest == ActiveRequest->HttpRequest))
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("Active request mismatch."));
		return;
	}

	if (bWasSuccessful)
	{
		int32 ResponseCode = InResponse->GetResponseCode();
		if (ResponseCode == EHttpResponseCodes::Ok)
		{
			FSMMatchmakingTicketResponse Response;
			if (FJsonObjectConverter::JsonObjectStringToUStruct(InResponse->GetContentAsString(), &Response, 0, 0))
			{
				ActiveMatchmakingTicket = NewObject<USMMatchmakingTicket>(this);
				ActiveMatchmakingTicket->MatchmakingType = ESMMatchmakingType::CustomMatch;
				ActiveMatchmakingTicket->TicketId = Response.TicketId;
				ActiveMatchmakingTicket->Status = Response.Status;
				ActiveMatchmakingTicket->Link = Response.Link;
				ActiveMatchmakingTicket->CreatedAt = GetWorld()->GetTimeSeconds();
				ActiveMatchmakingTicket->OnTicketUpdatedEvent.AddDynamic(this, &ThisClass::HandleMatchmakingTicketUpdated);
				StartTicketPolling();
				ActiveRequest->OnResponseEvent.Broadcast(true, ActiveMatchmakingTicket.Get(), LOCTABLE(LOCTABLE_ID, "Matchmaking_TicketCreated"));
				ActiveRequest.Reset();
				return;
			}
			UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleCreateCustomMatchResponse: Failed to parse response."));
		}
		else if (ResponseCode == EHttpResponseCodes::Denied)
		{
			UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleCreateCustomMatchResponse: User unauthorized."));
		}
		else if (ResponseCode == EHttpResponseCodes::BadRequest)
		{
			UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleCreateCustomMatchResponse: Bad request."));
		}
		else
		{
			UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleCreateCustomMatchResponse: Failed to create custom match with HTTP code: %d"), ResponseCode);
		}
	}
	else
	{
		UE_LOG(LogStereoMixMatchmaking, Error, TEXT("HandleCreateCustomMatchResponse: Failed to create custom match."));
	}

	ActiveRequest->OnResponseEvent.Broadcast(false, nullptr, LOCTABLE(LOCTABLE_ID, "Matchmaking_TicketCreateFailed"));
	ActiveRequest.Reset();
}

#undef LOCTABLE_ID
