// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DS_GameModeBase.h"
#include "GameLiftServerSDK.h"
#include "DS_LobbyGameMode.generated.h"

class UDS_GameInstanceSubsystem;
/**
 *  LobbyGameMode
 */
UCLASS()
class DEDICATEDSERVERS_API ADS_LobbyGameMode : public ADS_GameModeBase
{
	GENERATED_BODY()
public:
	ADS_LobbyGameMode();
	void CheckAndStartLobbyCountdown();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnCountdownTimerFinished(ECountdownTimerType Type) override;
	virtual void InitSeamlessTravelPlayer(AController* NewController) override; // 这个函数在SeamlessTravel时调用
	void CheckAndStopLobbyCountdown();
	virtual void Logout(AController* Exiting) override; // 玩家controller退出时调用
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;  // options是openlevel时传入的参数UGameSessionsManager::CreatePlayerSession_Response，Address是ip地址，UniqueId是玩家的唯一id
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = L"") override;  // after login, 这个函数会被调用。在这个函数中存储（登录后的）PlayerSessionId和Username
	
	UPROPERTY()
	ELobbyStatus LobbyStatus;

	UPROPERTY(EditDefaultsOnly)
	int32 MinPlayers;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DestinationMap;  // 存储map path，而不是map资源

private:
	
	UPROPERTY()
	TObjectPtr<UDS_GameInstanceSubsystem> DSGameInstanceSubsystem;

	UPROPERTY(EditDefaultsOnly)
	FCountdownTimerHandle LobbyCountdownTimer;

	void InitGameLift();
	void SetServerParameters(FServerParameters& OutServerParameters); // 设置游戏服务器需要的服务器参数
	void TryAcceptPlayerSession(const FString& PlayerSessionId, const FString& Username, FString& OutErrorMessage); // 尝试接受玩家的PlayerSession
	void AddPlayerInfoToLobbyState(AController* Player) const;
	void RemovePlayerInfoFromLobbyState(AController* Player) const;
};


