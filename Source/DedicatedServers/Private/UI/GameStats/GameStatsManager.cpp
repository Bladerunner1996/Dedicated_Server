// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameStats/GameStatsManager.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Data/API/APIData.h"
#include "DedicatedServers/DedicatedServers.h"
#include "Interfaces/IHttpRequest.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "GameplayTags/DedicatedServersTags.h"
#include "Interfaces/IHttpResponse.h"
#include "Player/DSLocalPlayerSubsystem.h"

void UGameStatsManager::RecordMatchStats(const FDSRecordMatchStatsInput& RecordMatchStatsInput)
{
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(FDSRecordMatchStatsInput::StaticStruct(), &RecordMatchStatsInput, JsonString);

	GEngine->AddOnScreenDebugMessage(-1, 600.f, FColor::Red, JsonString);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest(); 
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameStatsAPI::RecordMatchStats); 
	Request->OnProcessRequestComplete().BindUObject(this, &UGameStatsManager::RecordMatchStats_Response); 
	Request->SetURL(APIUrl);
	Request->SetVerb("POST"); 
	Request->SetHeader("Content-Type", "application/json");
	Request->SetContentAsString(JsonString);
	Request->ProcessRequest();
}

void UGameStatsManager::RetrieveMatchStats()
{
	RetrieveMatchStatsStatusMessage.Broadcast(TEXT("Retrieving Match Stats..."), true);  // 广播状态消息，发送请求时，表示正在获取游戏统计数据
	UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetDSLocalPlayerSubsystem();
	if (!IsValid(LocalPlayerSubsystem)) return;
	check(APIData);

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();  // 创建HTTP请求
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameStatsAPI::RetrieveMatchStats);  // 获取API的URL
	Request->OnProcessRequestComplete().BindUObject(this, &UGameStatsManager::RetrieveMatchStats_Response);  // 绑定请求完成的回调函数
	Request->SetURL(APIUrl);
	Request->SetVerb("POST");  // 设置请求方法为POST
	Request->SetHeader("Content-Type", "application/json");  // 设置请求头
	
	TMap<FString, FString> Params = {
		{TEXT("accessToken"), LocalPlayerSubsystem->GetAuthenticationResult().AccessToken} // 从LocalPlayerSubsystem获取AccessToken
	};
	const FString Content = SerializeJsonContent(Params); // 将TMap键值对数据转换为 JSON 字符串。
	
	Request->SetContentAsString(Content);  // 设置请求体为Json字符串
	Request->ProcessRequest();
	
}



void UGameStatsManager::RecordMatchStats_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogDedicatedServers, Error, TEXT("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@RecordMatchStats: Request failed"));
	}
	TSharedPtr<FJsonObject> JsonObject;    // 创建一个json对象，用于存储返回的json数据
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		ContainsErrors(JsonObject);
	}
}

void UGameStatsManager::RetrieveMatchStats_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		OnRetrieveMatchStatsResponseReceived.Broadcast(FDSRetrieveMatchStatsResponse());  // 失败时广播一个空的结构体
		RetrieveMatchStatsStatusMessage.Broadcast(TEXT("Fail to retrieve stats"), false);
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;    // 创建一个json对象，用于存储返回的json数据
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			OnRetrieveMatchStatsResponseReceived.Broadcast(FDSRetrieveMatchStatsResponse());
			RetrieveMatchStatsStatusMessage.Broadcast(TEXT("Contains Errors"), false);
			return;
		}

		FDSRetrieveMatchStatsResponse RetrieveMatchStatsResponse;  // 成功后返回的数据
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &RetrieveMatchStatsResponse);  // 反序列化json数据，将json数据转换为Response结构体
		RetrieveMatchStatsResponse.Dump();

		OnRetrieveMatchStatsResponseReceived.Broadcast(RetrieveMatchStatsResponse);  // 广播获取游戏统计数据的response 到 任何监听这个事件的widgets
		RetrieveMatchStatsStatusMessage.Broadcast(TEXT(""), true);  // 广播状态消息, 收到数据后，就清空这个消息
	}
}

void UGameStatsManager::UpdateLeaderboard(const TArray<FString>& WinnerUsername)
{
	check(APIData);

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();  // 创建HTTP请求
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameStatsAPI::UpdateLeaderboard);  // 获取API的URL
	Request->OnProcessRequestComplete().BindUObject(this, &UGameStatsManager::UpdateLeaderboard_Response);  // 绑定请求完成的回调函数
	Request->SetURL(APIUrl);
	Request->SetVerb("POST");  // 设置请求方法为POST
	Request->SetHeader("Content-Type", "application/json");  // 设置请求头
	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());  // 创建一个json对象
	TArray<TSharedPtr<FJsonValue>> PlayerIdJsonArray;
	
	// 将每个WinnerUsername添加到json数组中
	for (const FString& Username : WinnerUsername)
	{
		PlayerIdJsonArray.Add(MakeShareable(new FJsonValueString(Username)));  
	}
	JsonObject->SetArrayField(TEXT("playerIds"), PlayerIdJsonArray);  // 设置json对象的字段为 playerIds。这里的字段名要和lambda函数中的字段名一致。
	FString Content;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);  // 创建一个json字符串写入器
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);  // 将json对象序列化为字符串
	
	Request->SetContentAsString(Content);  // 设置请求体为Json字符串
	Request->ProcessRequest();
}



void UGameStatsManager::UpdateLeaderboard_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;    // 创建一个json对象，用于存储返回的json数据
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			return;
		}
	}
	OnUpdateLeaderboardSucceeded.Broadcast();  // 广播更新排行榜成功的事件
}

void UGameStatsManager::RetrieveLeaderboard()
{
	RetrieveLeaderboardStatusMessage.Broadcast(TEXT("Retrieving Leaderboard..."), false);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest(); 
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameStatsAPI::RetrieveLeaderboard); 
	Request->OnProcessRequestComplete().BindUObject(this, &UGameStatsManager::RetrieveLeaderboard_Response); 
	Request->SetURL(APIUrl);
	Request->SetVerb("GET"); 
	Request->SetHeader("Content-Type", "application/json");
	// 这里不需要设置请求体，因为是GET请求   Request->SetContentAsString(JsonString);
	Request->ProcessRequest();
}

void UGameStatsManager::RetrieveLeaderboard_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		RetrieveLeaderboardStatusMessage.Broadcast(TEXT("Fail to retrieve leaderboard"), false);
		UE_LOG(LogDedicatedServers, Error, TEXT("RetrieveLeaderboard: Request failed"));
		return;
	}

	TArray<FDSLeaderboardItem> LeaderboardItems;  // 用于存储 Http request 返回的排行榜数据
	TSharedPtr<FJsonObject> JsonObject;    // 创建一个json对象，用于存储返回的json数据
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());//
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			RetrieveLeaderboardStatusMessage.Broadcast(TEXT("JsonObject: Contains Errors."),false);
			return;
		}
		const TArray<TSharedPtr<FJsonValue>>* LeaderboardJsonArray;
		if (JsonObject->TryGetArrayField(TEXT("Leaderboard"), LeaderboardJsonArray)) // 把Leadboard字段的json数组转换为 TArray<TSharedPtr<FJsonValue>>* 类型的 LeaderboardJsonArray
		{
			for (const TSharedPtr<FJsonValue>& ItemValue : *LeaderboardJsonArray)  // *号解引用，遍历这个json数组中的每个元素。TArray是一个模板类，不是一个C++原生数组，所以解引用返回的是对象，不是首地址元素。
			{
				TSharedPtr<FJsonObject> ItemObject = ItemValue->AsObject();
				if (ItemObject.IsValid())
				{
					FDSLeaderboardItem Item;
					if (FJsonObjectConverter::JsonObjectToUStruct(ItemObject.ToSharedRef(), &Item))  // 第二个参数是一个指针类型。static bool JsonObjectToUStruct(const TSharedRef<FJsonObject>& JsonObject, OutStructType* OutStruct
					{
						LeaderboardItems.Add(Item);
					}
					else
					{
						UE_LOG(LogDedicatedServers, Error, TEXT("Fail to parse leaderboard item"));
					}
				}
			}
		}
	}
	OnRetrieveLeaderboard.Broadcast(LeaderboardItems);  // 广播获取排行榜的事件，传递排行榜数据
	RetrieveLeaderboardStatusMessage.Broadcast(TEXT(""),false);
}
