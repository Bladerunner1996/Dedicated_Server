// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/APITest/APITestManager.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Data/API/APIData.h"
#include "GameplayTags/DedicatedServersTags.h"
#include "Interfaces/IHttpResponse.h"
#include "UI/HTTP/HTTPRequestTypes.h"

void UAPITestManager::ListFleets()
{
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();  // 创建一个请求
	//TSharedRef 是一个智能指针，不许为空
	Request->OnProcessRequestComplete().BindUObject(this, &UAPITestManager::ListFleets_Response);  // 绑定回调函数
	
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameSessionsAPI::ListFleets);

	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->ProcessRequest();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("List Fleets Request Made"));
}

// 反序列化json数据，将json数据转换为FDSListFleetsResponse结构体，并广播出去
void UAPITestManager::ListFleets_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("List Fleets Response Received"));

	TSharedPtr<FJsonObject> JsonObject;    // 创建一个json对象，用于存储返回的json数据
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());   // 创建一个json读取器，用于读取Response中的json数据
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))  // 将json数据反序列化到JsonObject中
	{
		if (ContainsErrors(JsonObject))  // 判断返回的json数据中是否包含错误信息
		{
			OnListFleetsResponseReceivedDelegate.Broadcast(FDSListFleetsResponse{}, false);  // 如果包含错误信息，广播一个空的FDSListFleetsResponse结构体
			return;
		}
		DumpMetaData(JsonObject);  // 打印MetaData信息
		
		// Payload 信息
		FDSListFleetsResponse ListFleetsResponse;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &ListFleetsResponse);  // 将JsonObject中的数据转换为FDSListFleetsResponse结构体
		ListFleetsResponse.Dump();

		OnListFleetsResponseReceivedDelegate.Broadcast(ListFleetsResponse, true);  // 广播FDSListFleetsResponse结构体
	}
}
