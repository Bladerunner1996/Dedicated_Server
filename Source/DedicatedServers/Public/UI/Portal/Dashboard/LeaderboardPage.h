// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "LeaderboardPage.generated.h"

struct FDSLeaderboardItem;
class UTextBlock;
class ULeaderboardCard;
class UScrollBox;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ULeaderboardPage : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION()  // 绑定这个函数到 DashboardOverlay 的GamestatsManager 的 RetrieveLeaderboard 响应。 接受 response 中广播的 TArray<FDSLeaderboardItem>& 数据
	void PopulateLeaderboard(TArray<FDSLeaderboardItem>& Leaderboard);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_Leaderboard;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatusMessage;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULeaderboardCard> LeaderboardCardClass;

	UFUNCTION()
	void SetStatusMessage(const FString& StatusMessage, bool bShouldResetWidgets);

private:
	void CalculateLeaderboardPlace(TArray<FDSLeaderboardItem>& OutLeaderboard);
};

