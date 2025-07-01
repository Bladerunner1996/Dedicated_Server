// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/Interfaces/HUDManagement.h"
#include "PortalHUD.generated.h"

class UDashboardOverlay;
class USigninOverlay;
/**
 *  登录界面HUD
 *  为什么要用IHUDManagement接口？因为HUD依赖SigninOverlay和DashboardOverlay，这两个类又依赖PortalManager。PortalManager在signin成功时，需要调用HUD的函数切换界面，HUD需要调用SigninOverlay和DashboardOverlay的函数。
 *  这样就形成了一个循环依赖。为了解决这个问题，我们使用接口来解耦。这样，PortalManager调用HUD的函数时，只需要调用上层的接口函数，而不需要依赖HUD类。
 */
UCLASS()
class DEDICATEDSERVERS_API APortalHUD : public AHUD, public IHUDManagement
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USigninOverlay> SigninOverlayClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDashboardOverlay> DashboardOverlayClass;

	// 继承自IHUDManagement接口
	virtual void OnSignIn() override;
	virtual void OnSignOut() override;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	TObjectPtr<USigninOverlay> SigninOverlay;

	UPROPERTY()
	TObjectPtr<UDashboardOverlay> DashboardOverlay;
};
