// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DS_MatchGameMode.h"

#include "Player/DSPlayerController.h"
#include "Player/DS_MatchPlayerState.h"
#include "UI/GameStats/GameStatsManager.h"

ADS_MatchGameMode::ADS_MatchGameMode()
{
	bUseSeamlessTravel = true;
	MatchStatus = EMatchStatus::WaitingForPlayers;
	PreMatchTimer.Type = ECountdownTimerType::PreMatch;
	MatchTimer.Type = ECountdownTimerType::Match;
	PostMatchTimer.Type = ECountdownTimerType::PostMatch;
}

// PostLogin在seamless travel时不会调用。所以只在这里StartCountdownTimer(PreMatchTimer)，只在pie中login时生效
void ADS_MatchGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (MatchStatus == EMatchStatus::WaitingForPlayers)
	{
		// 如果当前是等待玩家状态，开始预备阶段
		MatchStatus = EMatchStatus::PreMatch;
		StartCountdownTimer(PreMatchTimer);
	}
}

// 从match到lobby时是seamless travel，会调用这个函数
void ADS_MatchGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
	Super::InitSeamlessTravelPlayer(NewController);
	if (MatchStatus == EMatchStatus::WaitingForPlayers)
	{
		// 如果当前是等待玩家状态，开始预备阶段
		MatchStatus = EMatchStatus::PreMatch;
		StartCountdownTimer(PreMatchTimer);
	}
}

void ADS_MatchGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	RemovePlayerSession(Exiting);
}

void ADS_MatchGameMode::OnCountdownTimerFinished(ECountdownTimerType Type)
{
	Super::OnCountdownTimerFinished(Type);

	if (Type == ECountdownTimerType::PreMatch) // 游戏前倒计时结束
	{
		StopCountdownTimer(PreMatchTimer);
		MatchStatus = EMatchStatus::Match;
		StartCountdownTimer(MatchTimer);
		SetClientInputEnabled(true); // 开启输入，玩家可以自由移动，开火
	}
	if (Type == ECountdownTimerType::Match) // 游戏结束
	{
		StopCountdownTimer(MatchTimer);
		MatchStatus = EMatchStatus::PostMatch;
		StartCountdownTimer(PostMatchTimer);
		SetClientInputEnabled(false); // 禁用输入，玩家不能移动，开火
		OnMatchEnded();  // 这里调用的是实际对象的OnMatchEnded（）？也即 FPSTemplate/Game/ShooterGameModeBase.h 中的 OnMatchEnded 函数吗？
	}
	if (Type == ECountdownTimerType::PostMatch)
	{
		// 结束游戏，无缝传送到lobby
		StopCountdownTimer(PostMatchTimer);
		MatchStatus = EMatchStatus::SeamlessTravelling;
		TrySeamlessTravel(LobbyMap);
	}
}

void ADS_MatchGameMode::BeginPlay()
{
	Super::BeginPlay();

	GameStatsManager = NewObject<UGameStatsManager>(this, GameStatsManagerClass);
	GameStatsManager->OnUpdateLeaderboardSucceeded.AddDynamic(this, &ADS_MatchGameMode::OnLeaderboardUpdated); // 绑定更新排行榜成功的事件
}

void ADS_MatchGameMode::SetClientInputEnabled(bool bEnabled)   // 遍历所有的PlayerController，调用Client_SetInputEnabled函数
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Iterator->Get());
		if (IsValid(DSPlayerController))
		{
			DSPlayerController->Client_SetInputEnabled(bEnabled);    // GameMode存在server上，调用PlayerController的Client_SetInputEnabled函数
		}
	}
}

// 游戏结束时，通知所有Player States，记录他们的 Match Stats。如果没有中间层 DS_MatchPlayerState，那么就要知道具体的游戏中 MatchPlayerState的类型，那样这个DS module就不通用了。
// 遍历所有的PlayerController，调用OnMatchEnded函数

void ADS_MatchGameMode::EndMatchForPlayerStates()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Iterator->Get()); IsValid(DSPlayerController))
		{
			if (ADS_MatchPlayerState* MatchPlayerState = DSPlayerController->GetPlayerState<ADS_MatchPlayerState>(); IsValid(MatchPlayerState))
			{
				MatchPlayerState->OnMatchEnded(DSPlayerController->Username); // 这里调用的是实际上的对象 AMatchPlayerState 的 OnMatchEnded 函数
			}
		}
	}
}

void ADS_MatchGameMode::OnMatchEnded()
{
}

// 这个函数实际上是给子类调用的。也就是 FPSTemplate 的ShooterGameModeBase类
void ADS_MatchGameMode::UpdateLeaderboard(const TArray<FString>& LeaderboardNames)
{
	if (IsValid(GameStatsManager))
	{
		GameStatsManager->UpdateLeaderboard(LeaderboardNames);  // 发送Http请求，收到response后会调用 OnLeaderboardUpdated 函数，再更新Player stats
	}
}

void ADS_MatchGameMode::OnLeaderboardUpdated()
{
	EndMatchForPlayerStates();
}

