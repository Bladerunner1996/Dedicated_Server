// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HTTPRequestManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAPIStatusMessage,const FString&, StatusMessage, bool, bShouldResetWidgets);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAPIRequestSucceeded);

class UAPIData;
class FJonsonObject;
class UDSLocalPlayerSubsystem;
/**
 * 
 */
UCLASS(Blueprintable)
class DEDICATEDSERVERS_API UHTTPRequestManager : public UObject
{
	GENERATED_BODY()
public:

	UDSLocalPlayerSubsystem* GetDSLocalPlayerSubsystem() const;
protected:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAPIData> APIData;

	bool ContainsErrors(const TSharedPtr<FJsonObject>& JsonObject) const;
	void DumpMetaData(const TSharedPtr<FJsonObject>& JsonObject) const;

	// 用于将键值对数据转换为 JSON 字符串，方便在网络请求中传递数据。
	FString SerializeJsonContent(const TMap<FString, FString>& Params);

};
