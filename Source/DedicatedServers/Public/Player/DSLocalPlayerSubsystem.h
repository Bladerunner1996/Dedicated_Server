// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "DSLocalPlayerSubsystem.generated.h"

class IPortalManagement;
/**
 * 为什么要用这个类？因为这个类是一个子系统，是一个持久的对象，可以在游戏运行时一直存在（在travel between level 和 to server时一直存在），而不是像PortalManager那样，只在登录界面存在。
 * 游戏期间，需要定期刷新token，所以需要一个持久的对象来存储token。
 */
UCLASS()
class DEDICATEDSERVERS_API UDSLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	void InitializeTokens(const FDSAuthenticationResult& InAuthenticationResult, TScriptInterface<IPortalManagement> PortalManagement); // 初始化登录tokens
	void SetRefreshTokenTimer();  // 设置定时刷新token
	void UpdateTokens(const FString& AccessToken, const FString& IdToken);  // 更新tokens
	FDSAuthenticationResult GetAuthenticationResult() const { return AuthenticationResult; }  // 获取tokens

	FString Username{};
	FString Email{};
	FString Password{};
	
private:
	UPROPERTY()
	FDSAuthenticationResult AuthenticationResult;

	UPROPERTY()
	TScriptInterface<IPortalManagement> PortalManagementInterface; // 用于调用PortalManager的RefreshTokens方法。 TScriptInterface是一个模板类，用于绑定接口，还可以暴露给蓝图。

	float TokenRefreshInterval =  2700.f;   // 刷新token的时间间隔，单位：秒（75%。AccessToken和IdToken过期时间1h）
	FTimerHandle RefreshTimer;
};
