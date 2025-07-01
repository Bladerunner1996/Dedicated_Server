// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HTTP/HTTPRequestManager.h"

#include "JsonObjectConverter.h"
#include "DedicatedServers/DedicatedServers.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "Player/DSLocalPlayerSubsystem.h"

UDSLocalPlayerSubsystem* UHTTPRequestManager::GetDSLocalPlayerSubsystem() const
{
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(LocalPlayerController->Player);
		if (IsValid(LocalPlayer))
		{
			UDSLocalPlayerSubsystem* DSLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UDSLocalPlayerSubsystem>();
			return DSLocalPlayerSubsystem;
		}
	}
	return nullptr;
}

bool UHTTPRequestManager::ContainsErrors(const TSharedPtr<FJsonObject>& JsonObject) const
{
	if (JsonObject->HasField(TEXT("errorType")) || JsonObject->HasField(TEXT("errorMessage")))  // 判断JsonObject中是否有"errorType"或"errorMessage"字段
	{
		FString ErrorType = JsonObject->HasField(TEXT("errorType")) ? JsonObject->GetStringField(TEXT("errorType")) : TEXT("Unknown Error");
		FString ErrorMessage = JsonObject->HasField(TEXT("errorMessage")) ? JsonObject->GetStringField(TEXT("errorMessage")) : TEXT("Unknown Error Message");

		UE_LOG(LogDedicatedServers, Error, TEXT("Error Type: %s"), *ErrorType);
		UE_LOG(LogDedicatedServers, Error, TEXT("Error Message: %s"), *ErrorMessage);

		return true;
	}

	if (JsonObject->HasField(TEXT("$fault")))  // 判断JsonObject中是否有"fault"字段
	{
		FString ErrorType = JsonObject->HasField(TEXT("name")) ? JsonObject->GetStringField(TEXT("name")) : TEXT("Unknown Error");
		UE_LOG(LogDedicatedServers, Error, TEXT("Error Type: %s"), *ErrorType);
		return true;
	}
	
	return false;
}

void UHTTPRequestManager::DumpMetaData(const TSharedPtr<FJsonObject>& JsonObject) const
{
	if (JsonObject->HasField(TEXT("$metadata")))   // 判断JsonObject中是否有"$metadata"字段
	{
		TSharedPtr<FJsonObject> MetaDataJsonObject = JsonObject->GetObjectField(TEXT("$metadata"));
		FDSMetaData DSMetaData;
		FJsonObjectConverter::JsonObjectToUStruct(MetaDataJsonObject.ToSharedRef(), &DSMetaData);  // 将MetaDataJsonObject中的数据转换为FDSMetaData结构体
		DSMetaData.Dump();
	}
}

FString UHTTPRequestManager::SerializeJsonContent(const TMap<FString, FString>& Params)
{
	TSharedPtr<FJsonObject> ContentJsonObject = MakeShareable(new FJsonObject);
	for (const auto& Param : Params)
	{
		ContentJsonObject->SetStringField(Param.Key, Param.Value);
	}
	///ContentJsonObject->SetStringField(TEXT("playerId"), PlayerId);  // 设置json数据, 要和API文档中的参数名一致（lambda的event input）
	
	FString Content;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&Content);  // 创建一个JsonWriter，用于将Json数据写入到Content中
	FJsonSerializer::Serialize(ContentJsonObject.ToSharedRef(), JsonWriter); //FJsonSerializer::Serialize 将 ContentJsonObject 的内容序列化为 JSON 格式，并通过 JsonWriter 输出到 Content。
	return Content;
}

