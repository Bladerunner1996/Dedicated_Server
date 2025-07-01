// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/APITest/APITestHUD.h"

#include "UI/APITest/APITestOverlay.h"

void AAPITestHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetOwningPlayerController();
	if (IsValid(PlayerController) && APITestOverlayClass)  //为什么判断为空的形式不一样？
	{
		APITestOverlay = CreateWidget<UAPITestOverlay>(PlayerController, APITestOverlayClass);
		APITestOverlay->AddToViewport();
	}
}
