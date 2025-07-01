// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Dashboard/DashboardOverlay.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "UI/GameStats/GameStatsManager.h"
#include "UI/Portal/Dashboard/CareerPage.h"
#include "UI/Portal/Dashboard/GamePage.h"
#include "UI/Portal/Dashboard/LeaderboardPage.h"

void UDashboardOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	GameStatsManager = NewObject<UGameStatsManager>(this, GameStatsManagerClass);
	GameStatsManager->OnRetrieveMatchStatsResponseReceived.AddDynamic(CareerPage, &UCareerPage::OnRetrieveMatchStats); // 绑定接收数据响应函数
	GameStatsManager->RetrieveMatchStatsStatusMessage.AddDynamic(CareerPage, &UCareerPage::SetStatusMessage); // 绑定状态消息函数
	
	GameStatsManager->OnRetrieveLeaderboard.AddDynamic(LeaderboardPage, &ULeaderboardPage::PopulateLeaderboard); // 绑定接收排行榜数据响应函数
	GameStatsManager->RetrieveLeaderboardStatusMessage.AddDynamic(LeaderboardPage, &ULeaderboardPage::SetStatusMessage); // 绑定状态消息函数
	
	Button_Game->OnClicked.AddDynamic(this, &UDashboardOverlay::ShowGamePage);
	Button_Career->OnClicked.AddDynamic(this, &UDashboardOverlay::ShowCareerPage);
	Button_Leaderboard->OnClicked.AddDynamic(this, &UDashboardOverlay::ShowLeaderboardPage);

	ShowGamePage();
}

void UDashboardOverlay::ShowGamePage()
{
	EnableButtons();
	Button_Game->SetIsEnabled(false);
	WidgetSwitcher->SetActiveWidget(GamePage);
}

void UDashboardOverlay::ShowCareerPage()
{
	EnableButtons();
	Button_Career->SetIsEnabled(false);
	WidgetSwitcher->SetActiveWidget(CareerPage);
	GameStatsManager->RetrieveMatchStats(); // 发送HTTP请求, 调用请求完成的回调函数，广播玩家数据。触发CareerPage的OnRetrieveMatchStats函数，显示玩家数据
}

void UDashboardOverlay::ShowLeaderboardPage()
{
	EnableButtons();
	Button_Leaderboard->SetIsEnabled(false);
	WidgetSwitcher->SetActiveWidget(LeaderboardPage);
	GameStatsManager->RetrieveLeaderboard();
	
}

void UDashboardOverlay::EnableButtons() const
{
	Button_Career->SetIsEnabled(true);
	Button_Game->SetIsEnabled(true);
	Button_Leaderboard->SetIsEnabled(true);
}
