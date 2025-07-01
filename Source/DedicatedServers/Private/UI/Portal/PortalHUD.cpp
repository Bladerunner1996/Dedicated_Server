// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/PortalHUD.h"
#include "UI/Portal/Signin/SigninOverlay.h"
#include "Blueprint/UserWidget.h"
#include "UI/Portal/Dashboard/DashboardOverlay.h"


void APortalHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* OwningPlayerController = GetOwningPlayerController();
	SigninOverlay = CreateWidget<USigninOverlay>(OwningPlayerController, SigninOverlayClass);
	if (IsValid(SigninOverlay))
	{
		SigninOverlay->AddToViewport();
	}

	FInputModeGameAndUI InputModeData;
	OwningPlayerController->SetInputMode(InputModeData);
	OwningPlayerController->bShowMouseCursor = true;
}

void APortalHUD::OnSignIn()
{
	if (IsValid(SigninOverlay))
	{
		SigninOverlay->RemoveFromParent();
	}
	DashboardOverlay = CreateWidget<UDashboardOverlay>(GetOwningPlayerController(), DashboardOverlayClass);
	if (IsValid(DashboardOverlay))
	{
		DashboardOverlay->AddToViewport();
	}
}

void APortalHUD::OnSignOut()
{
	if (IsValid(DashboardOverlay))
	{
		DashboardOverlay->RemoveFromParent();
	}
	SigninOverlay = CreateWidget<USigninOverlay>(GetOwningPlayerController(), SigninOverlayClass);
	if (IsValid(SigninOverlay))
	{
		SigninOverlay->AddToViewport();
	}
}
