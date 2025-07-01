// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "DS_GameState.generated.h"

// 广播LobbyState初始化完成的事件到客户端
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyStateInitialized, ALobbyState*, LobbyState);

class ALobbyState;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ADS_GameState : public AGameState
{
	GENERATED_BODY()
public:
	ADS_GameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // 网络复制需要的函数

	UPROPERTY(ReplicatedUsing=OnRep_LobbyState)
	TObjectPtr<ALobbyState> LobbyState;

	UPROPERTY(BlueprintAssignable)
	FOnLobbyStateInitialized OnLobbyStateInitialized;

protected:
	
	virtual void BeginPlay() override; // GameState开始时，就创建LobbyState，触发LobbyState的 replication，触发OnRep_LobbyState函数
	void CreateLobbyState();

private:

	UFUNCTION()
	void OnRep_LobbyState();
};
