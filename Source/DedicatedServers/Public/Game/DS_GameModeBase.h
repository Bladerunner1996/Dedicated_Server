// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Types/DSTypes.h"
#include "DS_GameModeBase.generated.h"

/**
 * 一个游戏模式基类，用于处理倒计时Timer。 LobbyGameMode和 MatchGameMode都继承自这个类。
 */
UCLASS()
class DEDICATEDSERVERS_API ADS_GameModeBase : public AGameMode
{
	GENERATED_BODY()
protected:
	void StartCountdownTimer(FCountdownTimerHandle& CountdownTimerHandle);  // 绑定CountdownTimerHandle上的 delegate，并SetTimer
	void StopCountdownTimer(FCountdownTimerHandle& CountdownTimerHandle);  // 解绑CountdownTimerHandle上的 delegate，并ClearTimer
	void UpdateCountdownTimer(FCountdownTimerHandle& CountdownTimerHandle);  // 更新CountdownTimerHandle上的时间
	virtual void OnCountdownTimerFinished(ECountdownTimerType Type);
	void TrySeamlessTravel(TSoftObjectPtr<UWorld> DestinationMap);  // 尝试SeamlessTravel
	void RemovePlayerSession(AController* Exiting);  // 删除PlayerSession
};
