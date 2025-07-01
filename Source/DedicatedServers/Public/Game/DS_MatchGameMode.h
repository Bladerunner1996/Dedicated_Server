// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DS_GameModeBase.h"
#include "DS_MatchGameMode.generated.h"

class UGameStatsManager;
/**
 * match相关的功能
 */
UCLASS()
class DEDICATEDSERVERS_API ADS_MatchGameMode : public ADS_GameModeBase
{
	GENERATED_BODY()
public:
	ADS_MatchGameMode();

	//PostLogin仅会在玩家登录时调用（即从dashboard到lobby），而不会在从一个地图切换到另一个地图时调用
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void InitSeamlessTravelPlayer(AController* NewController) override;  // 这是SeamlessTravel时调用
	virtual void Logout(AController* Exiting) override;

	UPROPERTY()
	EMatchStatus MatchStatus;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameStatsManager> GameStatsManagerClass;

	
protected:
	virtual void OnCountdownTimerFinished(ECountdownTimerType Type) override;
	virtual void BeginPlay() override;

	void SetClientInputEnabled(bool bEnabled); // 通知所有客户端设置输入是否可用
	void EndMatchForPlayerStates();
	virtual void OnMatchEnded();  // 在 FPSTemplate/Game/ShooterGameModeBase.h override

	void UpdateLeaderboard(const TArray<FString>& LeaderboardNames);

	UPROPERTY(EditDefaultsOnly)
	FCountdownTimerHandle PreMatchTimer;
	
	UPROPERTY(EditDefaultsOnly)
	FCountdownTimerHandle MatchTimer;

	UPROPERTY(EditDefaultsOnly)
	FCountdownTimerHandle PostMatchTimer;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> LobbyMap;   // 结束游戏后无缝传送到Lobby地图

	UFUNCTION()
	void OnLeaderboardUpdated();

private:
	
	UPROPERTY()
	TObjectPtr<UGameStatsManager> GameStatsManager;
};

