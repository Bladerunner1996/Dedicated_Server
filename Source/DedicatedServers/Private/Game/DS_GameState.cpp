// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DS_GameState.h"

#include "Lobby/LobbyState.h"
#include "Net/UnrealNetwork.h"

ADS_GameState::ADS_GameState()
{
	bReplicates = true;  // 允许网络复制
}

void ADS_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADS_GameState, LobbyState);  // 复制 LobbyState 属性
}

void ADS_GameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		CreateLobbyState();
		OnLobbyStateInitialized.Broadcast(LobbyState);  // just incase we want to bind to this delegate on the server
	}
	
}

void ADS_GameState::CreateLobbyState()
{
	if (UWorld* World = GetWorld(); IsValid(World))
	{
		FActorSpawnParameters SpawnParams;
		
		LobbyState = World->SpawnActor<ALobbyState>(
			ALobbyState::StaticClass(),
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			SpawnParams);
		
		if (IsValid(LobbyState))
		{
			LobbyState->SetOwner(this);
		}
	}
}

void ADS_GameState::OnRep_LobbyState()
{
	OnLobbyStateInitialized.Broadcast(LobbyState); // 广播 LobbyState 初始化完成，到客户端
}
