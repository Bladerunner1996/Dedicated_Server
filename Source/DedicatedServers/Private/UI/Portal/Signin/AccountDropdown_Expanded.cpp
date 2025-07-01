// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Signin/AccountDropdown_Expanded.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/DSLocalPlayerSubsystem.h"
#include "UI/Portal/PortalManager.h"

void UAccountDropdown_Expanded::NativeConstruct()
{
	Super::NativeConstruct();

	PortalManager = NewObject<UPortalManager>(this, PortalManagerClass);
	Button_SignOut->OnClicked.AddDynamic(this, &UAccountDropdown_Expanded::SignOutButton_OnClicked);
	Button_SignOut->OnHovered.AddDynamic(this, &UAccountDropdown_Expanded::SignOutButton_Hover);
	Button_SignOut->OnUnhovered.AddDynamic(this, &UAccountDropdown_Expanded::SignOutButton_UnHover);

	// 获取存储在 LocalPlayerSubsystem中的 Email 并显示
	UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		TextBlock_Email->SetText(FText::FromString(LocalPlayerSubsystem->Email));
	}
}

void UAccountDropdown_Expanded::NativePreConstruct()
{
	Super::NativePreConstruct();
	SignOutButton_UnHover();
	SetSignOutButtonTransparent();
}

void UAccountDropdown_Expanded::SignOutButton_Hover()
{
	TextBlock_SignOutButtonText->SetColorAndOpacity(HoveredTextColor);
}

void UAccountDropdown_Expanded::SignOutButton_UnHover()
{
	TextBlock_SignOutButtonText->SetColorAndOpacity(UnHoveredTextColor);
}

void UAccountDropdown_Expanded::SignOutButton_OnClicked()
{
	Button_SignOut->SetIsEnabled(false);

	check(PortalManager);
	//获取存储在 LocalPlayerSubsystem中的 AccessToken。（sign-in response中返回的数据）
	UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		FDSAuthenticationResult AuthResult = LocalPlayerSubsystem->GetAuthenticationResult();
		PortalManager->SignOut(AuthResult.AccessToken);
	}
	
}

void UAccountDropdown_Expanded::SetSignOutButtonTransparent()
{
	FButtonStyle Style;
	FSlateBrush Brush;
	Brush.TintColor = FSlateColor(FLinearColor(0.f,0.f,0.f,0.f));
	Style.Disabled = Brush;
	Style.Hovered = Brush;
	Style.Normal = Brush;
	Style.Pressed = Brush;
	Button_SignOut->SetStyle(Style);
}

UDSLocalPlayerSubsystem* UAccountDropdown_Expanded::GetLocalPlayerSubsystem()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController) && IsValid(PlayerController->GetLocalPlayer()))
	{
		UDSLocalPlayerSubsystem* LocalPlayerSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<UDSLocalPlayerSubsystem>();
		if (IsValid(LocalPlayerSubsystem))
		{
			return LocalPlayerSubsystem;
		}
	}
	return nullptr;
}
