// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/APITest/RecordMatchStatsWidget.h"

#include "Components/Button.h"
#include "Player/DSLocalPlayerSubsystem.h"
#include "UI/GameStats/GameStatsManager.h"
#include "UI/HTTP/HTTPRequestTypes.h"

void URecordMatchStatsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	GameStatsManager = NewObject<UGameStatsManager>(this, GameStatsManagerClass);
	Button_RecordMatchStats->OnClicked.AddDynamic(this, &ThisClass::RecordMatchStatsButtonClicked);
}

void URecordMatchStatsWidget::RecordMatchStatsButtonClicked()
{
	check(GameStatsManager);
	// UDSLocalPlayerSubsystem* LocalPlayerSubsystem = GetOwningLocalPlayer()->GetSubsystem<UDSLocalPlayerSubsystem>(); // 获取LocalPlayerSubsystem以获取Username
	// if (!IsValid(LocalPlayerSubsystem)) return;
	const FString Username = TEXT("qq");  // 获取Username

	FDSRecordMatchStatsInput RecordMatchStatsInput;  // RecordMatchStats的输入结构体
	RecordMatchStatsInput.username = Username;  // 设置Username
	RecordMatchStatsInput.matchStats.hits = 1;  // 设置游戏统计数据
	RecordMatchStatsInput.matchStats.misses = 2;
	RecordMatchStatsInput.matchStats.headShotElims = 5;

	GameStatsManager->RecordMatchStats(RecordMatchStatsInput);  // 调用RecordMatchStats函数
}
