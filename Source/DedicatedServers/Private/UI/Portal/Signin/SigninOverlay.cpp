// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Signin/SigninOverlay.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "DedicatedServers/DedicatedServers.h"
#include "Player/DSLocalPlayerSubsystem.h"
#include "UI/API/GameSessions/JoinGame.h"
#include "UI/Portal/PortalManager.h"
#include "UI/Portal/Signin/ConfirmSignUpPage.h"
#include "UI/Portal/Signin/SignInPage.h"
#include "UI/Portal/Signin/SignUpPage.h"
#include "UI/Portal/Signin/SuccessConfirmedPage.h"

void USigninOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	check(PortalManagerClass);
	PortalManager = NewObject<UPortalManager>(this, PortalManagerClass);  // 创建一个PortalManager对象。

	check(IsValid(SignInPage));
	//check(IsValid(SignInPage->Button_SignIn)); 其实不需要检查，UPROPERTY(meta = (BindWidget))会自动检查
	SignInPage->Button_SignIn->OnClicked.AddDynamic(this,&ThisClass::SignInButtonClicked);
	SignInPage->Button_SignUp->OnClicked.AddDynamic(this, &ThisClass::ShowSignUpPage);
	SignInPage->Button_Quit->OnClicked.AddDynamic(PortalManager, &UPortalManager::QuitGame); // AddDynamic的第一个参数是要执行操作的对象，第二个参数是该对象的成员函数指针
	PortalManager->SignInStatusMessageDelegate.AddDynamic(SignInPage, &USignInPage::UpdateStatusMessage); // 绑定委托，更新SignInPage的状态信息

	check(IsValid(SignUpPage));
	SignUpPage->Button_SignUp->OnClicked.AddDynamic(this, &ThisClass::SignUpButtonClicked);
	SignUpPage->Button_Back->OnClicked.AddDynamic(this, &ThisClass::ShowSignInPage);
	PortalManager->SignUpStatusMessageDelegate.AddDynamic(SignUpPage, &USignUpPage::UpdateStatusMessage);   // 绑定委托，更新SignUpPage的状态信息
	PortalManager->OnSignUpSucceeded.AddDynamic(this, &ThisClass::OnSignUpSucceeded);  // 绑定函数，注册成功后，切换到ConfirmSignUpPage

	check(IsValid(ConfirmSignUpPage));
	ConfirmSignUpPage->Button_Confirm->OnClicked.AddDynamic(this, &ThisClass::ConfirmButtonClicked);
	ConfirmSignUpPage->Button_Back->OnClicked.AddDynamic(this, &ThisClass::ShowSignUpPage);
	PortalManager->ConfirmStatusMessageDelegate.AddDynamic(ConfirmSignUpPage, &UConfirmSignUpPage::UpdateStatusMessage);  // 绑定委托，更新ConfirmSignUpPage的状态信息
	PortalManager->OnConfirmSucceeded.AddDynamic(this, &ThisClass::OnConfirmSucceeded);  // 绑定函数，确认成功后，切换到SuccessConfirmedPage
	
	check(IsValid(SuccessConfirmedPage));
	SuccessConfirmedPage->Button_OK->OnClicked.AddDynamic(this, &ThisClass::ShowSignInPage);

	ShowSignInPage();
	AutoSignIn();
}


void USigninOverlay::AutoSignIn()
{
	if (UDSLocalPlayerSubsystem* DSLocalPlayerSubsystem = PortalManager->GetDSLocalPlayerSubsystem())
	{
		const FString& Username = DSLocalPlayerSubsystem->Username;
		const FString& Password = DSLocalPlayerSubsystem->Password;
		if (Username.IsEmpty() || Password.IsEmpty()) return;

		SignInPage->Button_SignIn->SetIsEnabled(false);
		PortalManager->SignIn(Username, Password);
	}
}

void USigninOverlay::ShowSignInPage()
{
	SignInPage->Button_SignIn->SetIsEnabled(true);
	WidgetSwitcher->SetActiveWidget(SignInPage);
}

void USigninOverlay::ShowSignUpPage()
{
	WidgetSwitcher->SetActiveWidget(SignUpPage);
}

void USigninOverlay::ShowConfirmSignUpPage()
{
	WidgetSwitcher->SetActiveWidget(ConfirmSignUpPage);
}

void USigninOverlay::ShowSuccessConfirmedPage()
{
	WidgetSwitcher->SetActiveWidget(SuccessConfirmedPage);
}

void USigninOverlay::SignInButtonClicked()
{
	const FString UserName = SignInPage->TextBox_Username->GetText().ToString();
	const FString Password = SignInPage->TextBox_Password->GetText().ToString();
	if (UDSLocalPlayerSubsystem* DSLocalPlayerSubsystem = PortalManager->GetDSLocalPlayerSubsystem())
	{
		DSLocalPlayerSubsystem->Password = Password;   // 第一登录时，存储密码，以便后续自动登录
	}
	
	PortalManager->SignIn(UserName, Password);
}

void USigninOverlay::SignUpButtonClicked()
{
	const FString UserName = SignUpPage->TextBox_Username->GetText().ToString();
	const FString Password = SignUpPage->TextBox_Password->GetText().ToString();
	const FString Email = SignUpPage->TextBox_Email->GetText().ToString();
	PortalManager->SignUp(UserName, Password, Email);
}

void USigninOverlay::ConfirmButtonClicked()
{
	const FString ConfirmationCode = ConfirmSignUpPage->TextBox_ConfirmationCode->GetText().ToString();
	ConfirmSignUpPage->Button_Confirm->SetIsEnabled(false);
	PortalManager->Confirm(ConfirmationCode);
}

void USigninOverlay::OnSignUpSucceeded()
{
	SignUpPage->ClearTextBoxes();
	ConfirmSignUpPage->TextBlock_Destination->SetText(FText::FromString(PortalManager->LastSignUpResponse.CodeDeliveryDetails.Destination));  // 在ConfirmSignUpPage中显示邮箱地址
	ShowConfirmSignUpPage();
}

void USigninOverlay::OnConfirmSucceeded()
{
	ConfirmSignUpPage->ClearTextBoxes();
	ShowSuccessConfirmedPage();
}
