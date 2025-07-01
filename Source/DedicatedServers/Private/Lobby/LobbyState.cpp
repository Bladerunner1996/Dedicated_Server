// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyState.h"
#include "Net/UnrealNetwork.h"  // 网络复制需要的头文件

ALobbyState::ALobbyState()
{
	PrimaryActorTick.bCanEverTick = false;  // 不需要每帧更新
	bReplicates = true;  // 允许网络复制
	bAlwaysRelevant = true;  // 始终相关，有些actor距离过远可能会不被复制，这个actor需要始终被复制
}

void ALobbyState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyState, PlayerInfoArray);  // (Class, PropertyName)  // 复制 PlayerInfoArray 属性
}

void ALobbyState::AddPlayerInfo(const FLobbyPlayerInfo& PlayerInfo)
{
	PlayerInfoArray.AddPlayer(PlayerInfo);
}

void ALobbyState::RemovePlayerInfo(const FString& Username)
{
	PlayerInfoArray.RemovePlayer(Username);
}

void ALobbyState::OnRep_LobbyPlayerInfo()
{
	FLobbyPlayerInfoDelta Delta = ComputePlayerInfoDelta(LastPlayerInfoArray.Players, PlayerInfoArray.Players);
	for (const auto& PlayerInfo : Delta.AddedPlayers)
	{
		OnPlayerInfoAdded.Broadcast(PlayerInfo);  // 广播新增玩家信息
	}
	for (const auto& PlayerInfo : Delta.RemovedPlayers)
	{
		OnPlayerInfoRemoved.Broadcast(PlayerInfo);  // 广播删除玩家信息
	}

	LastPlayerInfoArray = PlayerInfoArray;  // 每次复制时，存储上一次的 PlayerInfoArray
}

// 比较两个 FLobbyPlayerInfo 数组，返回新增和删除的玩家信息
FLobbyPlayerInfoDelta ALobbyState::ComputePlayerInfoDelta(const TArray<FLobbyPlayerInfo>& OldArray, const TArray<FLobbyPlayerInfo>& NewArray)
{
	FLobbyPlayerInfoDelta Delta;

	TMap<FString, const FLobbyPlayerInfo*> OldMap;
	TMap<FString, const FLobbyPlayerInfo*> NewMap;

	for (const auto& PlayerInfo : OldArray)
	{
		OldMap.Add(PlayerInfo.UserName, &PlayerInfo);
	}

	for (const auto& PlayerInfo : NewArray)
	{
		NewMap.Add(PlayerInfo.UserName, &PlayerInfo);
	}

	for (const auto& OldPlayerInfo : OldArray)
	{
		if (!NewMap.Contains(OldPlayerInfo.UserName))
		{
			Delta.RemovedPlayers.Add(OldPlayerInfo);
		}
	}

	for (const auto& NewPlayerInfo : NewArray)
	{
		if (!OldMap.Contains(NewPlayerInfo.UserName))
		{
			Delta.AddedPlayers.Add(NewPlayerInfo);
		}
	}
	return Delta;
}

