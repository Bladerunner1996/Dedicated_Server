// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SigninOverlay.generated.h"

class USuccessConfirmedPage;
class UConfirmSignUpPage;
class USignUpPage;
class USignInPage;
class UPortalManager;
class UJoinGame;
class UWidgetSwitcher;
class UButton;
/**
 *  登录界面
 */
UCLASS()
class DEDICATEDSERVERS_API USigninOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPortalManager> PortalManagerClass;  //为什么要用subclassof，因为这样可以在蓝图中指定PortalManager的蓝图类，而不是固定的PortalManagerC++类。毕竟蓝图类里才有 API DATA。

	UPROPERTY(meta = (BindWidget))// 绑定到蓝图要求名字一致
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;
protected:
	virtual void NativeConstruct() override; // 初始化函数（而非构造函数）
	
private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USignInPage> SignInPage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USignUpPage> SignUpPage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UConfirmSignUpPage> ConfirmSignUpPage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USuccessConfirmedPage> SuccessConfirmedPage;
	
	UPROPERTY()
	TObjectPtr<UPortalManager> PortalManager;

	void AutoSignIn();

	UFUNCTION()
	void ShowSignInPage();
	UFUNCTION()
	void ShowSignUpPage();
	UFUNCTION()
	void ShowConfirmSignUpPage();
	UFUNCTION()
	void ShowSuccessConfirmedPage();

	UFUNCTION()
	void SignInButtonClicked();

	UFUNCTION()
	void SignUpButtonClicked();

	UFUNCTION()
	void ConfirmButtonClicked();

	UFUNCTION()
	void OnSignUpSucceeded();

	UFUNCTION()
	void OnConfirmSucceeded();
};
