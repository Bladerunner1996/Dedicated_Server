// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "UI/HTTP/HTTPRequestManager.h"
#include "APITestManager.generated.h"

// 用于广播ListFleets请求的响应(到APITestOverlay)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnListFleetsResponseReceived, const FDSListFleetsResponse&, ListFleetsResponse, bool, bWasSuccessful);
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UAPITestManager : public UHTTPRequestManager
{
	GENERATED_BODY()
public:

	UFUNCTION()
	void ListFleets();

	UPROPERTY()
	FOnListFleetsResponseReceived OnListFleetsResponseReceivedDelegate;

	// Request->OnProcessRequestComplete()  绑定回调函数
	void ListFleets_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
