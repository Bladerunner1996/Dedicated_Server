// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DS_GameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/DSPlayerController.h"
#include "GameLiftServerSDK.h"

// 有两个Timer Handle，一个是倒计时结束的，一个是更新倒计时的。分别绑定不同的委托。委托又绑定不同的lambda函数。
// 两个关键函数：BindWeakLambda() 和 SetTimer()。一个是委托绑定函数，一个是TimerHandle绑定委托。
void ADS_GameModeBase::StartCountdownTimer(FCountdownTimerHandle& CountdownTimerHandle)
{
	// finished timer
	CountdownTimerHandle.TimerFinishedDelegate.BindWeakLambda(this,[&]() // BindWeakLambda() 用于绑定一个弱引用的lambda函数，不会增加引用计数。 & 表示以引用的方式传递参数
	{
		// 服务器timer结束时，ClearTimer，解绑，通知客户端timer结束。并调用一些事件。
		OnCountdownTimerFinished(CountdownTimerHandle.Type);
	});
	
	GetWorldTimerManager().SetTimer(
		CountdownTimerHandle.TimerFinishedHandle,
		CountdownTimerHandle.TimerFinishedDelegate,
		CountdownTimerHandle.CountdownTime,
		false); // (handle, delegate, time, loop)

	// update timer
	CountdownTimerHandle.TimerUpdateDelegate.BindWeakLambda(this,[&]()
	{
		// 服务器更新timer时，通知上面的所有客户端更新timer
		UpdateCountdownTimer(CountdownTimerHandle);
	});

	GetWorldTimerManager().SetTimer(
		CountdownTimerHandle.TimerUpdateHandle,
		CountdownTimerHandle.TimerUpdateDelegate,
		CountdownTimerHandle.CountdownUpdateInterval,
		true);
	UpdateCountdownTimer(CountdownTimerHandle); // 立即更新一次，而不是等到第一次UpdateTimerHandle才更新
}

void ADS_GameModeBase::StopCountdownTimer(FCountdownTimerHandle& CountdownTimerHandle)
{
	CountdownTimerHandle.State = ECountdownTimerState::Stopped;
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle.TimerFinishedHandle);
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle.TimerUpdateHandle);
	// 解绑委托，为什么？避免访问已销毁的对象，避免持有旧的回调逻辑
	if (CountdownTimerHandle.TimerFinishedDelegate.IsBound())
	{
		CountdownTimerHandle.TimerFinishedDelegate.Unbind();
	}
	if (CountdownTimerHandle.TimerUpdateDelegate.IsBound())
	{
		CountdownTimerHandle.TimerUpdateDelegate.Unbind();
	}
	// 服务器停止timer时，通知上面的所有客户端停止timer
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Iterator->Get());
		if (IsValid(DSPlayerController))
		{
			DSPlayerController->Client_TimerStopped(0.f, CountdownTimerHandle.Type);
		}
	}
}

void ADS_GameModeBase::UpdateCountdownTimer(FCountdownTimerHandle& CountdownTimerHandle)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) // 对所有的PlayerController进行遍历，通知所有客户端更新timer
	{
		ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Iterator->Get());
		if (IsValid(DSPlayerController))
		{
			const float CountdownTimerLeft = CountdownTimerHandle.CountdownTime - GetWorldTimerManager().GetTimerElapsed(CountdownTimerHandle.TimerFinishedHandle); // 剩余时间
			DSPlayerController->Client_TimerUpdated(CountdownTimerLeft, CountdownTimerHandle.Type);
		}
	}
}

void ADS_GameModeBase::OnCountdownTimerFinished(ECountdownTimerType Type)
{
}

void ADS_GameModeBase::TrySeamlessTravel(TSoftObjectPtr<UWorld> DestinationMap)
{
	const FString MapName = DestinationMap.ToSoftObjectPath().GetAssetName();
	if (GIsEditor) //编辑器模式
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DestinationMap);
	}
	else // build版本中
	{
		GetWorld()->ServerTravel(MapName);
	}
}

void ADS_GameModeBase::RemovePlayerSession(AController* Exiting)
{
	ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Exiting);
	if (!IsValid(DSPlayerController)) return;

#if WITH_GAMELIFT
	const FString& PlayerSessionId = DSPlayerController->PlayerSessionId;
	if (!PlayerSessionId.IsEmpty())
	{
		Aws::GameLift::Server::RemovePlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));  // 删除PlayerSession
	}
#endif
}
