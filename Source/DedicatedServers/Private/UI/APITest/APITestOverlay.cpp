// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/APITest/APITestOverlay.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "UI/API/ListFleets/FleetId.h"
#include "UI/API/ListFleets/ListFleetsBox.h"
#include "UI/APITest/APITestManager.h"
#include "UI/HTTP/HTTPRequestTypes.h"

void UAPITestOverlay::NativeConstruct()
{
	Super::NativeConstruct();
	check(APITestManagerClass);
	APITestManager = NewObject<UAPITestManager>(this, APITestManagerClass);  // 创建一个APITestManager对象

	check(ListFleetsBox);
	check(ListFleetsBox->Button_ListFleets);
	ListFleetsBox->Button_ListFleets->OnClicked.AddDynamic(this, &UAPITestOverlay::ListFleetsButtonClicked);
}

void UAPITestOverlay::ListFleetsButtonClicked()
{
	check(APITestManager);
	APITestManager->OnListFleetsResponseReceivedDelegate.AddDynamic(this, &UAPITestOverlay::OnListFleetsResponseReceived);  // 绑定ResponseReceived回调函数
	APITestManager->ListFleets();   // 发送ListFleets请求，触发Request->OnProcessRequestComplete，广播OnListFleetsResponseReceivedDelegate，会调用OnListFleetsResponseReceived回调函数
	ListFleetsBox->Button_ListFleets->SetIsEnabled(false);
}

void UAPITestOverlay::OnListFleetsResponseReceived(const FDSListFleetsResponse& ListFleetsResponse, bool bWasSuccessful)
{
	// 调用回调函数的同时，解绑回调函数, 防止重复绑定
	if (APITestManager->OnListFleetsResponseReceivedDelegate.IsAlreadyBound(this, &UAPITestOverlay::OnListFleetsResponseReceived))
	{
		APITestManager->OnListFleetsResponseReceivedDelegate.RemoveDynamic(this, &UAPITestOverlay::OnListFleetsResponseReceived);
	}
	ListFleetsBox->ScrollBox_ListFleets->ClearChildren();  // 清空ScrollBox中的所有子控件
	if (bWasSuccessful)
	{
		for (const FString& FleetId : ListFleetsResponse.FleetIds)
		{
			UFleetId* FleetIdWidget = CreateWidget<UFleetId>(this, FleetIdWidgetClass);
			FleetIdWidget->TextBlock_FleetId->SetText(FText::FromString(FleetId));
			ListFleetsBox->ScrollBox_ListFleets->AddChild(FleetIdWidget);
		}
	}
	else
	{
		UFleetId* FleetIdWidget = CreateWidget<UFleetId>(this, FleetIdWidgetClass);  // 创建一个FleetIdWidget
		FleetIdWidget->TextBlock_FleetId->SetText(FText::FromString("Something went wrong"));
		ListFleetsBox->ScrollBox_ListFleets->AddChild(FleetIdWidget);
	}
	ListFleetsBox->Button_ListFleets->SetIsEnabled(true);
}
