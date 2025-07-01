// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DSLocalPlayerSubsystem.h"

#include "UI/Portal/Interfaces/PortalManagement.h"

void UDSLocalPlayerSubsystem::InitializeTokens(const FDSAuthenticationResult& InAuthenticationResult, TScriptInterface<IPortalManagement> PortalManagement)
{
	AuthenticationResult = InAuthenticationResult;
	PortalManagementInterface = PortalManagement;
	SetRefreshTokenTimer();
}

void UDSLocalPlayerSubsystem::SetRefreshTokenTimer()
{
	UWorld* World = GetWorld();
	if (IsValid(World) && IsValid(PortalManagementInterface.GetObject()))  // .GetObject()获取实现了IPortalManagement接口的对象，这里即PortalManager。离开登录界面后，PortalManager会被销毁，这里IsValid(PortalManagementInterface.GetObject())会返回false
	{
		FTimerDelegate RefreshDelegate;
		RefreshDelegate.BindLambda([this](){PortalManagementInterface->RefreshTokens(AuthenticationResult.RefreshToken);});
		World->GetTimerManager().SetTimer(RefreshTimer, RefreshDelegate, TokenRefreshInterval, false);  // 流程：SignIn_Response -> InitializeTokens -> SetRefreshTokenTimer -> RefreshTokens_Response -> UpdateTokens -> SetRefreshTokenTimer
	}
}

void UDSLocalPlayerSubsystem::UpdateTokens(const FString& AccessToken, const FString& IdToken)
{
	AuthenticationResult.AccessToken = AccessToken;
	AuthenticationResult.IdToken = IdToken;
	AuthenticationResult.Dump();
	SetRefreshTokenTimer();
}
