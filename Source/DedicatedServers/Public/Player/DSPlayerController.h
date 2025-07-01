// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Types/DSTypes.h"
#include "DSPlayerController.generated.h"

//server timer更新的时候，这些委托在client上broadcast，通知client更新timer
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTimerStateChangedDelegate, float, Time, ECountdownTimerType, Type);
/**
 * 服务器上的玩家控制器，用于更新timer等
 */
UCLASS()
class DEDICATEDSERVERS_API ADSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADSPlayerController();
	virtual void BeginPlay() override;

	virtual void ReceivedPlayer() override; // 重写ReceivedPlayer，用于在玩家控制器被创建时发送ping请求
	virtual void OnRep_PlayerState() override;  // Rep_Notify
	virtual void PostSeamlessTravel() override;
	
	UFUNCTION(Client, Reliable) // 在server调用Client_RPC，在client广播委托，更新timer。
	void Client_TimerUpdated(float TimeLeft, ECountdownTimerType Type) const; // 用于广播timer更新的委托

	UFUNCTION(Client, Reliable)
	void Client_TimerStopped(float TimeLeft, ECountdownTimerType Type) const; // 用于广播timer停止的委托

	UFUNCTION(Client, Reliable)
	void Client_SetInputEnabled(bool bEnabled); // 用于设置输入是否可用

	UPROPERTY(BlueprintAssignable)
	FOnTimerStateChangedDelegate OnTimerUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnTimerStateChangedDelegate OnTimerStopped;

	UPROPERTY(BlueprintReadOnly)
	FString Username;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerSessionId;
	
protected:

	UFUNCTION(Server, Reliable)  // Server关键字表示这个函数是在客户端调用，服务器执行
	void Server_Ping(float TimeOfRequest); // 测试服务器到客户端的延迟

	UFUNCTION(Client, Reliable)
	void Client_Pong(float TimeOfRequest); // 还挺有意思的，pingpong
private:

	float SingleTripTime;
};
