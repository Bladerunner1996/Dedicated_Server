// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameSessions/GameSessionsManager.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Data/API/APIData.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTags/DedicatedServersTags.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DSLocalPlayerSubsystem.h"
#include "UI/HTTP/HTTPRequestTypes.h"

void UGameSessionsManager::JoinGameSession()
{
	BroadcastJoinGameSessionMessage.Broadcast("Searching for game session...", false);
	
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();  // 创建一个Http请求,TSharedRef 是一个智能指针，不许为空
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::FindOrCreateGameSession_Response);  // 绑定回调函数到请求完成事件
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameSessionsAPI::FindOrCreateGameSession);  // 通过Tag获取APIUrl

	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetDSLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		Request->SetHeader(TEXT("Authorization"), LocalPlayerSubsystem->GetAuthenticationResult().AccessToken);  // 把AccessToken中的custom scope放到Header的"Authorization"中
	}
	
	Request->ProcessRequest();
}

void UGameSessionsManager::FindOrCreateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		BroadcastJoinGameSessionMessage.Broadcast("Failed to find or create game session.", true);
	}

	TSharedPtr<FJsonObject> JsonObject;    // 创建一个json对象，用于存储返回的json数据
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());   // 创建一个json读取器，用于读取Response中的json数据
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			BroadcastJoinGameSessionMessage.Broadcast("Something Went Wrong: Error", true);
		}

		FDSGameSession GameSession;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &GameSession);

		const FString GameSessionId = GameSession.GameSessionId;
		const FString GameSessionStatus = GameSession.Status;
		HandleGameSessionStatus(GameSessionId,GameSessionStatus);
		
	}
}

void UGameSessionsManager::CreatePlayerSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		BroadcastJoinGameSessionMessage.Broadcast("Failed to create player session.", true);
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;    // 创建一个json对象，用于存储返回的json数据
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());   // 创建一个json读取器，用于读取Response中的json数据
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			BroadcastJoinGameSessionMessage.Broadcast("Something Went Wrong: Error", true);
			return;
		}

		FDSPlayerSession PlayerSession;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &PlayerSession);  // 反序列化json数据，将json数据转换为FDSPlayerSession结构体
		PlayerSession.Dump();

		// 连接到游戏服务器后，InputMode改变，鼠标隐藏
		APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (IsValid(LocalPlayerController))
		{
			FInputModeGameOnly InputModeData;
			LocalPlayerController->SetInputMode(InputModeData);
			LocalPlayerController->SetShowMouseCursor(false);
		}

		const FString Options = "?PlayerSessionId=" + PlayerSession.PlayerSessionId + "?Username=" + PlayerSession.PlayerId;  // 作为OpenLevel的参数，传递PlayerSessionId和Username到游戏服务器
		
		const FString IpAndPort = PlayerSession.IpAddress + TEXT(":") + FString::FromInt(PlayerSession.Port);
		const FName Address(*IpAndPort);
		UGameplayStatics::OpenLevel(this, Address, true, Options);  // hard travel。从客户端 travel 到游戏服务器game session
		
	}
}


FString UGameSessionsManager::GetPlayerUniqueId() const
{
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{
		APlayerState* LocalPlayerState = LocalPlayerController->GetPlayerState<APlayerState>();
		if (IsValid(LocalPlayerState) && LocalPlayerState->GetUniqueId().IsValid())
		{
			const FString UniqueId = TEXT("Player_") + FString::FromInt(LocalPlayerState->GetUniqueID());
			return UniqueId;
		}
	}
	return FString();
}

void UGameSessionsManager::HandleGameSessionStatus(const FString& SessionId, const FString& Status)
{
	if (Status.Equals(TEXT("ACTIVE")))
	{
		BroadcastJoinGameSessionMessage.Broadcast("Found active GameSession. Creating a Player Session", false);

		if (UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetDSLocalPlayerSubsystem(); IsValid(LocalPlayerSubsystem))
		{
			TryCreatePlayerSession(LocalPlayerSubsystem->Username, SessionId);// 创建一个PlayerSession需要PlayerId和GameSessionId。把Username作为PlayerId传入，Username存储在DSLocalPlayerSubsystem中
		}
		
	}
	else if (Status.Equals(TEXT("ACTIVATING"))) // 延迟0.5秒后再次尝试。调用过程---JoinGameSession，FindOrCreateGameSession_Response，HandleGameSessionStatus
	{
		FTimerDelegate CreateSessionDelegate;
		CreateSessionDelegate.BindUObject(this, &ThisClass::JoinGameSession);
		APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (IsValid(LocalPlayerController))
		{
			LocalPlayerController->GetWorldTimerManager().SetTimer(CreateSessionTimer, CreateSessionDelegate, 0.5f, false);
		}
	}
	else
	{
		BroadcastJoinGameSessionMessage.Broadcast("Something went wrong: No Game Session", true);
	}
}

void UGameSessionsManager::TryCreatePlayerSession(const FString& PlayerId, const FString& GameSessionId)
{
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();  // 创建一个Http请求,TSharedRef 是一个智能指针，不许为空
	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::CreatePlayerSession_Response);  // 绑定回调函数到请求完成事件
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameSessionsAPI::CreatePlayerSession);  // 通过Tag获取APIUrl

	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TMap<FString, FString> Params = {
		{TEXT("playerId"), PlayerId},
		{TEXT("gameSessionId"), GameSessionId}
	};
	const FString Content = SerializeJsonContent(Params); // 将键值对数据转换为 JSON 字符串。
	
	Request->SetContentAsString(Content); // 设置请求内容
	Request->ProcessRequest(); // 发送请求, Request->OnProcessRequestComplete() 会在请求完成后调用绑定的回调函数
}
