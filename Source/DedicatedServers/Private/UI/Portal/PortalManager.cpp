// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/PortalManager.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Data/API/APIData.h"
#include "GameplayTags/DedicatedServersTags.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DSLocalPlayerSubsystem.h"
#include "UI/Interfaces/HUDManagement.h"
#include "GameFramework/HUD.h"


void UPortalManager::SignIn(const FString& Username, const FString& Password)
{
	SignInStatusMessageDelegate.Broadcast(TEXT("登录中..."), false);
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();  // 创建一个Http请求
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::SignIn_Response);  // 绑定回调函数到请求完成事件
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::PortalAPI::SignIn);  // 通过Tag获取APIUrl

	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	LastUsername = Username;
	TMap<FString, FString> Params = {
		{TEXT("username"), Username},
		{TEXT("password"), Password}
	};
	const FString Content = SerializeJsonContent(Params); // 将键值对数据转换为 JSON 字符串。
	Request->SetContentAsString(Content); // 设置请求内容
	Request->ProcessRequest();
}

void UPortalManager::SignIn_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		SignInStatusMessageDelegate.Broadcast(TEXT("Something went wrong."), true);
	}

	TSharedPtr<FJsonObject> JsonObject;    // 创建一个json对象，用于存储返回的json数据
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			SignInStatusMessageDelegate.Broadcast(TEXT("Something Went Wrong: Error"), true);
			return;
		}

		FDSInitiateAuthResponse InitiateAuthResponse;  // 登录成功后返回的数据
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &InitiateAuthResponse);  // 反序列化json数据，将json数据转换为FDSSignInResponse结构体
		InitiateAuthResponse.Dump();

		UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetDSLocalPlayerSubsystem();
		if (IsValid(LocalPlayerSubsystem))
		{
			LocalPlayerSubsystem->InitializeTokens(InitiateAuthResponse.AuthenticationResult, this);  // 将登录成功后返回的数据存储到DSLocalPlayerSubsystem中(1小时会过期，需要定时刷新，通常45分钟刷新一次)
			LocalPlayerSubsystem->Username = LastUsername;
			LocalPlayerSubsystem->Email = InitiateAuthResponse.email;
		}

		APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (IsValid(LocalPlayerController))
		{
			if (IHUDManagement* HUDManagement = Cast<IHUDManagement>(LocalPlayerController->GetHUD()))  // Cast 到接口
			{
				HUDManagement->OnSignIn();  // 登录成功后，调用HUD的OnSignIn函数，切换到Dashboard界面
			}
		}
		
	}
}

void UPortalManager::SignUp(const FString& Username, const FString& Password, const FString& Email)
{
	SignUpStatusMessageDelegate.Broadcast(TEXT("Create a new account..."), false);
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();  // 创建一个Http请求
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::SignUp_Response);  // 绑定回调函数到请求完成事件
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::PortalAPI::SignUp);  // 通过Tag获取APIUrl

	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	LastUsername = Username;
	TMap<FString, FString> Params = {
		{TEXT("username"), Username},
		{TEXT("password"), Password},
		{TEXT("email"), Email}
	};
	const FString Content = SerializeJsonContent(Params); // 将键值对数据转换为 JSON 字符串。
	Request->SetContentAsString(Content); // 设置请求内容
	Request->ProcessRequest();
}

void UPortalManager::SignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		SignUpStatusMessageDelegate.Broadcast(TEXT("Failed to create a new account."), true);
	}

	TSharedPtr<FJsonObject> JsonObject;    // 创建一个json对象，用于存储返回的json数据
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			SignUpStatusMessageDelegate.Broadcast(TEXT("Something Went Wrong: Error"), true);
			return;
		}
		
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &LastSignUpResponse);  // 反序列化json数据，将json数据转换为FDSSignUpResponse结构体
		
		OnSignUpSucceeded.Broadcast(); // 广播通知UI注册成功
	}
}

void UPortalManager::Confirm(const FString& ConfirmationCode)
{
	check(APIData);
	ConfirmStatusMessageDelegate.Broadcast(TEXT("验证中..."), false);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();  // 创建一个Http请求
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::Confirm_Response);  // 绑定回调函数到请求完成事件
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::PortalAPI::ConfirmSignUp);  // 通过Tag获取APIUrl

	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("PUT"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TMap<FString, FString> Params = {
		{TEXT("username"), LastUsername},
		{TEXT("confirmationCode"), ConfirmationCode}
	};
	const FString Content = SerializeJsonContent(Params); // 将键值对数据转换为 JSON 字符串。
	Request->SetContentAsString(Content); // 设置http请求内容
	Request->ProcessRequest();
}

void UPortalManager::Confirm_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ConfirmStatusMessageDelegate.Broadcast(TEXT("Failed to create a new account."), true);
	}

	TSharedPtr<FJsonObject> JsonObject;    // 创建一个json对象，用于存储返回的json数据
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsErrors(JsonObject))
		{
			if (JsonObject->HasField(TEXT("errorType")))
			{
				FString Exception = JsonObject->GetStringField(TEXT("errorType"));
				if (Exception.Equals(TEXT("CodeMismatchException")))
				{
					ConfirmStatusMessageDelegate.Broadcast(TEXT("验证码错误"), true);
					return;
				}
			}
			ConfirmStatusMessageDelegate.Broadcast(TEXT("Something Went Wrong: Error"), true);
			return;
		}
	}
	OnConfirmSucceeded.Broadcast(); // 广播通知UI验证成功
}



void UPortalManager::QuitGame()
{
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{
		UKismetSystemLibrary::QuitGame(this, LocalPlayerController, EQuitPreference::Quit, false);
	}
	
}

void UPortalManager::RefreshTokens(const FString& RefreshToken)
{
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();  // 创建一个Http请求
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::RefreshTokens_Response);  // 绑定回调函数到请求完成事件
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::PortalAPI::SignIn);  // 通过Tag获取APIUrl。SignIn和RefreshToken的APIUrl是一样的

	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	TMap<FString, FString> Params = {
		{TEXT("refreshToken"), RefreshToken}
	};
	const FString Content = SerializeJsonContent(Params); // 将键值对数据转换为 JSON 字符串。
	Request->SetContentAsString(Content); // 设置请求内容
	Request->ProcessRequest();
}

void UPortalManager::RefreshTokens_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
		FDSInitiateAuthResponse InitiateAuthResponse;  // 刷新成功后返回的数据
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &InitiateAuthResponse);  // 反序列化json数据，将json数据转换为FDSSignInResponse结构体

		UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetDSLocalPlayerSubsystem();
		if (IsValid(LocalPlayerSubsystem))
		{
			LocalPlayerSubsystem->UpdateTokens(InitiateAuthResponse.AuthenticationResult.AccessToken, InitiateAuthResponse.AuthenticationResult.IdToken);  // (刷新一次Token)
		}
		
	}
}

void UPortalManager::SignOut(const FString& AccessToken)
{
	check(APIData);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();  // 创建一个Http请求
	Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::SignOut_Response);  // 绑定回调函数到请求完成事件
	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::PortalAPI::SignOut);  // 通过Tag获取APIUrl

	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	
	TMap<FString, FString> Params = {
		{TEXT("accessToken"), AccessToken}
	};
	const FString Content = SerializeJsonContent(Params); // 将键值对数据转换为 JSON 字符串。
	Request->SetContentAsString(Content); // 设置请求内容
	Request->ProcessRequest();
}

void UPortalManager::SignOut_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
	if (UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetDSLocalPlayerSubsystem())
	{
		LocalPlayerSubsystem->Username.Empty();  // 清空用户名
		LocalPlayerSubsystem->Email.Empty();  // 清空邮箱
		LocalPlayerSubsystem->Password.Empty();  // 清空密码
		
	}
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{
		if (IHUDManagement* HUDManagement = Cast<IHUDManagement>(LocalPlayerController->GetHUD()))
		{
			HUDManagement->OnSignOut();  // 登出成功后，调用HUD的OnSignOut函数，切换到Signin界面
		}
	}
}





