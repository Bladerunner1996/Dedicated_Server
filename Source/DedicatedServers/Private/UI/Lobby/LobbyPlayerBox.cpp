// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Lobby/LobbyPlayerBox.h"

#include "Components/ScrollBox.h"
#include "Game/DS_GameState.h"
#include "Lobby/LobbyState.h"
#include "Lobby/LobbyPlayerInfo.h"
#include "UI/Lobby/PlayerLabel.h"

void ULobbyPlayerBox::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ADS_GameState* DSGameState = GetWorld()->GetGameState<ADS_GameState>();
	if (!IsValid(DSGameState)) return;

	if (IsValid(DSGameState->LobbyState)) // 如果是在服务器上，LobbyState已经创建完成
	{
		OnLobbyStateInitialized(DSGameState->LobbyState);
	}
	else  
	{
		DSGameState->OnLobbyStateInitialized.AddDynamic(this, &ULobbyPlayerBox::OnLobbyStateInitialized);
	}
}

void ULobbyPlayerBox::UpdatePlayerInfo(ALobbyState* LobbyState)
{
	// Clear existing player labels
	ScrollBox_PlayerInfo->ClearChildren();

	// Create and add player labels for each player in the lobby
	for (const FLobbyPlayerInfo& PlayerInfo : LobbyState->GetPlayers())
	{
		CreateAndAddPlayerLabel(PlayerInfo);
	}
}

void ULobbyPlayerBox::OnLobbyStateInitialized(ALobbyState* LobbyState)
{
	if (!IsValid(LobbyState)) return;

	LobbyState->OnPlayerInfoAdded.AddDynamic(this, &ULobbyPlayerBox::CreateAndAddPlayerLabel);
	LobbyState->OnPlayerInfoRemoved.AddDynamic(this, &ULobbyPlayerBox::OnPlayerRemoved);
	UpdatePlayerInfo(LobbyState);
}

void ULobbyPlayerBox::CreateAndAddPlayerLabel(const FLobbyPlayerInfo& PlayerInfo)
{
	if (FindPlayerLabel(PlayerInfo.UserName)) return;
	
	UPlayerLabel* PlayerLabel = CreateWidget<UPlayerLabel>(this, PlayerLabelClass);
	if (!IsValid(PlayerLabel)) return;

	PlayerLabel->SetUsername(PlayerInfo.UserName);
	ScrollBox_PlayerInfo->AddChild(PlayerLabel);
}

void ULobbyPlayerBox::OnPlayerRemoved(const FLobbyPlayerInfo& PlayerInfo)
{
	if (UPlayerLabel* PlayerLabel = FindPlayerLabel(PlayerInfo.UserName))
	{
		PlayerLabel->RemoveFromParent();
	}
}


UPlayerLabel* ULobbyPlayerBox::FindPlayerLabel(const FString& Username) const
{
	for (UWidget* Child : ScrollBox_PlayerInfo->GetAllChildren())
	{
		UPlayerLabel* PlayerLabel = Cast<UPlayerLabel>(Child);
		if (IsValid(PlayerLabel) && PlayerLabel->GetUsername() == Username)
		{
			return PlayerLabel;
		}
	}
	return nullptr;
}
