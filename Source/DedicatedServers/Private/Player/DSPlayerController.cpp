// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DSPlayerController.h"

ADSPlayerController::ADSPlayerController()
{
	SingleTripTime = 0.0f;
	Username = "";
	PlayerSessionId = "";
}

void ADSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_Standalone)
	{
		DisableInput(this);
	}
}

void ADSPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (GetNetMode() == NM_Standalone) return; // 如果是单机模式，直接返回
	// if (GetNetMode() == NM_ListenServer) Listen Server无所谓ping不ping，因为服务器在本地，调用是瞬间的，没有延迟
	if (IsLocalController()) // Dedicated Server，所有LocalController只存在客户端。只有本地控制器才发送ping请求 (即客户端)
	{
		Server_Ping(GetWorld()->GetTimeSeconds()); // 客户端调用Server_Ping向服务器发送ping请求，服务器执行Server_Ping调用Client_Pong向客户端发送pong响应，客户端执行Client_Pong计算单程时间
	}
}

void ADSPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (IsLocalController())
	{
		DisableInput(this);
	}
}

void ADSPlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	if (IsLocalController())
	{
		Server_Ping(GetWorld()->GetTimeSeconds());
		DisableInput(this);
	}
}

void ADSPlayerController::Client_SetInputEnabled_Implementation(bool bEnabled)
{
	if (bEnabled)
	{
		EnableInput(this);  // APlayerController的这两个函数被重写了（AShooterPlayerController），调用实际对象的重写版本
	}
	else
	{
		DisableInput(this);
	}
}

// server call, client 执行。 RPC
void ADSPlayerController::Client_TimerUpdated_Implementation(float TimeLeft, ECountdownTimerType Type) const
{
	OnTimerUpdated.Broadcast(TimeLeft - SingleTripTime, Type);  // 时间-单程时间，用于修正延迟
}

void ADSPlayerController::Client_TimerStopped_Implementation(float TimeLeft, ECountdownTimerType Type) const
{
	OnTimerStopped.Broadcast(TimeLeft - SingleTripTime, Type);
}

void ADSPlayerController::Server_Ping_Implementation(float TimeOfRequest)
{
	Client_Pong(TimeOfRequest);
}

void ADSPlayerController::Client_Pong_Implementation(float TimeOfRequest)
{
	const float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfRequest;
	SingleTripTime = RoundTripTime / 2.0f;  // 近似计算单程时间
}


