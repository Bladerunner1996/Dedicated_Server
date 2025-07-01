// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "UI/HTTP/HTTPRequestManager.h"
#include "GameSessionsManager.generated.h"


/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UGameSessionsManager : public UHTTPRequestManager
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FAPIStatusMessage BroadcastJoinGameSessionMessage;

	void JoinGameSession();

private:
	// Request->OnProcessRequestComplete()  绑定回调函数
	void FindOrCreateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void CreatePlayerSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	FString GetPlayerUniqueId() const;

	void HandleGameSessionStatus(const FString& SessionId, const FString& Status);
	void TryCreatePlayerSession(const FString& PlayerId, const FString& GameSessionId);
	FTimerHandle CreateSessionTimer;
};
