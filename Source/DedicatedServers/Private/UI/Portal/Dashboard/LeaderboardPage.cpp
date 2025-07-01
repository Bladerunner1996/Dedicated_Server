// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Dashboard/LeaderboardPage.h"

#include "Components/ScrollBox.h"
#include "UI/HTTP/HTTPRequestTypes.h"
#include "UI/Portal/Dashboard/LeaderboardCard.h"

void ULeaderboardPage::PopulateLeaderboard(TArray<FDSLeaderboardItem>& Leaderboard)
{
	ScrollBox_Leaderboard->ClearChildren();
	CalculateLeaderboardPlace(Leaderboard);
	
	for (const FDSLeaderboardItem& Item : Leaderboard)
	{
		ULeaderboardCard* LeaderboardCard = CreateWidget<ULeaderboardCard>(this, LeaderboardCardClass);
		if (IsValid(LeaderboardCard))
		{
			LeaderboardCard->SetPlayerInfo(Item.username, Item.matchWins, Item.place);
			ScrollBox_Leaderboard->AddChild(LeaderboardCard);
		}
	}
}

void ULeaderboardPage::CalculateLeaderboardPlace(TArray<FDSLeaderboardItem>& OutLeaderboard)
{
	OutLeaderboard.Sort([](const FDSLeaderboardItem& A, const FDSLeaderboardItem& B) -> bool
	{
		return A.matchWins > B.matchWins;  // 按照matchWins降序排序
	});
	int CurrentRank =1;
	for (int32 i = 0; i < OutLeaderboard.Num(); ++i)
	{
		if (i > 0 && OutLeaderboard[i].matchWins == OutLeaderboard[i - 1].matchWins)
		{
			OutLeaderboard[i].place = OutLeaderboard[i - 1].place;  // 如果当前玩家的matchWins与前一个玩家相同，则设置为前一个玩家的排名
		}
		else
		{
			OutLeaderboard[i].place = CurrentRank;  // 否则，设置当前玩家的排名
		}
		CurrentRank++;
	}
}

void ULeaderboardPage::SetStatusMessage(const FString& StatusMessage, bool bShouldResetWidgets)
{
	TextBlock_StatusMessage->SetText(FText::FromString(StatusMessage));
}
