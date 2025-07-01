// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DS_LobbyGameMode.h"
#include "Game/DS_GameInstanceSubsystem.h"
#include "DedicatedServers/DedicatedServers.h"
#include "Game/DS_GameState.h"
#include "Kismet/GameplayStatics.h"
#include "Lobby/LobbyState.h"
#include "Player/DSPlayerController.h"

ADS_LobbyGameMode::ADS_LobbyGameMode()
{
	bUseSeamlessTravel = true;
	LobbyStatus = ELobbyStatus::WaitingForPlayers;
	MinPlayers = 2;
	LobbyCountdownTimer.Type = ECountdownTimerType::Lobby;
}

// PostLogin仅会在玩家登录时调用（即从dashboard到lobby），而不会在从一个地图切换到另一个地图时调用
void ADS_LobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	CheckAndStartLobbyCountdown();
	UE_LOG(LogTemp, Warning, TEXT("ADS_LobbyGameMode::PostLogin for %s"), *NewPlayer->GetName());
}

// SeamlessTravel时调用 (比如从Match地图切换到Lobby地图)，第一次登录到lobby时不会调用
void ADS_LobbyGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
	Super::InitSeamlessTravelPlayer(NewController);
	CheckAndStartLobbyCountdown();

	if (LobbyStatus != ELobbyStatus::SeamlessTravelling)
	{
		AddPlayerInfoToLobbyState(NewController);  // 添加玩家信息到LobbyState
	}
	
	UE_LOG(LogTemp, Warning, TEXT("ADS_LobbyGameMode::InitSeamlessTravelPlayer for %s"), *NewController->GetName());
}

void ADS_LobbyGameMode::CheckAndStartLobbyCountdown()
{
	if (GetNumPlayers() >= MinPlayers && LobbyStatus == ELobbyStatus::WaitingForPlayers)  // 人够且在等人中
	{
		LobbyStatus = ELobbyStatus::CountdownToSeamlessTravel;
		StartCountdownTimer(LobbyCountdownTimer);
	}
}

void ADS_LobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	CheckAndStopLobbyCountdown();
	RemovePlayerSession(Exiting);
	if (LobbyStatus != ELobbyStatus::SeamlessTravelling)
	{
		RemovePlayerInfoFromLobbyState(Exiting);  // 删除玩家信息从LobbyState
	}
	UE_LOG(LogTemp, Warning, TEXT("ADS_LobbyGameMode::Logout for %s"), *Exiting->GetName());
}

void ADS_LobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	// 解析传入的参数
	const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
	const FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));

	TryAcceptPlayerSession(PlayerSessionId, Username, ErrorMessage);  // 尝试接受玩家的PlayerSession
	UE_LOG(LogTemp, Warning, TEXT("ADS_LobbyGameMode::PreLogin - PlayerSessionId: %s, Username: %s"), *PlayerSessionId, *Username);
}

void ADS_LobbyGameMode::TryAcceptPlayerSession(const FString& PlayerSessionId, const FString& Username, FString& OutErrorMessage)
{
	if (PlayerSessionId.IsEmpty() || Username.IsEmpty())
	{
		OutErrorMessage = TEXT("PlayerSessionId or Username is invalid");
		return;
	}
	
// Gamelift Server SDK functions
#if WITH_GAMELIFT
	Aws::GameLift::Server::Model::DescribePlayerSessionsRequest DescribePlayerSessionsRequest;
	DescribePlayerSessionsRequest.SetPlayerSessionId(TCHAR_TO_ANSI(*PlayerSessionId));

	const auto& DescribePlayerSessionOutcome =  Aws::GameLift::Server::DescribePlayerSessions(DescribePlayerSessionsRequest);
	if (!DescribePlayerSessionOutcome.IsSuccess())
	{
		OutErrorMessage = TEXT("DescribePlayerSessions failed");
		return;
	}
	const auto& DescribePlayerSessionResult = DescribePlayerSessionOutcome.GetResult();
	int32 Count = 0;
	const Aws::GameLift::Server::Model::PlayerSession* PlayerSessions = DescribePlayerSessionResult.GetPlayerSessions(Count);
	if (PlayerSessions == nullptr || Count == 0)
	{
		OutErrorMessage = TEXT("GetPlayerSessions failed");
		return;
	}

	for (int32 i = 0; i < Count; i++)
	{
		const Aws::GameLift::Server::Model::PlayerSession& PlayerSession = PlayerSessions[i];
		if (!Username.Equals(PlayerSession.GetPlayerId())) continue;   // 寻找Username对应的PlayerSession
		if (PlayerSession.GetStatus() != Aws::GameLift::Server::Model::PlayerSessionStatus::RESERVED)  // Reserved状态的意思是已经创建了PlayerSession，但是还没有被接受
		{
			OutErrorMessage = FString::Printf(TEXT("Session for %s is not in RESERVED state"), *Username);
			return;
		}
		const auto& AcceptPlayerSessionOutcome = Aws::GameLift::Server::AcceptPlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));
		OutErrorMessage = AcceptPlayerSessionOutcome.IsSuccess() ? "" : FString::Printf(TEXT("AcceptPlayerSession %s failed"), *Username);
	}
#endif
}

void ADS_LobbyGameMode::AddPlayerInfoToLobbyState(AController* Player) const
{
	ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Player);
	ADS_GameState* DSGameState = GetGameState<ADS_GameState>();
	if (IsValid(DSGameState) && IsValid(DSGameState->LobbyState) && IsValid(DSPlayerController))
	{
		FLobbyPlayerInfo PlayerInfo(DSPlayerController->Username);
		DSGameState->LobbyState->AddPlayerInfo(PlayerInfo);  // 添加玩家信息到LobbyState
	}
}

void ADS_LobbyGameMode::RemovePlayerInfoFromLobbyState(AController* Player) const
{
	ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(Player);
	ADS_GameState* DSGameState = GetGameState<ADS_GameState>();
	if (IsValid(DSGameState) && IsValid(DSGameState->LobbyState) && IsValid(DSPlayerController))
	{
		DSGameState->LobbyState->RemovePlayerInfo(DSPlayerController->Username);  // 删除玩家信息从LobbyState
	}
}

// 在prelogin之后，在postlogin之前调用。解析Options，（分配PlayerState给NewPlayerController，在super中）
FString ADS_LobbyGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	FString InitializedString = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	// 解析传入的参数
	const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
	const FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));

	// 这里是用来存储PlayerSessionId和Username的。以便logout时使用
	// 不用DSLocalPlayerSubsystem存储persist data，因为是那client用的。现在是在server上，gamemode中，persist data可以存储在DS Player Controller中。DSPlayerController在seamless travel时是不会被销毁的
	if (ADSPlayerController* DSPlayerController = Cast<ADSPlayerController>(NewPlayerController); IsValid(DSPlayerController))
	{
		DSPlayerController->PlayerSessionId = PlayerSessionId;
		DSPlayerController->Username = Username;
	}
	if (LobbyStatus != ELobbyStatus::SeamlessTravelling)
	{
		AddPlayerInfoToLobbyState(NewPlayerController);  // 添加玩家信息到LobbyState
	}
	UE_LOG(LogTemp, Warning, TEXT("ADS_LobbyGameMode::PreLogin - PlayerSessionId: %s, Username: %s"), *PlayerSessionId, *Username);
	return InitializedString;
}

void ADS_LobbyGameMode::CheckAndStopLobbyCountdown()
{
	if (GetNumPlayers() - 1 < MinPlayers && LobbyStatus == ELobbyStatus::CountdownToSeamlessTravel)  // 少人且在倒计时中
	{
		LobbyStatus = ELobbyStatus::WaitingForPlayers;
		StopCountdownTimer(LobbyCountdownTimer);
	}
}

void ADS_LobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	InitGameLift();
}

void ADS_LobbyGameMode::OnCountdownTimerFinished(ECountdownTimerType Type)
{
	Super::OnCountdownTimerFinished(Type);

	if (Type == ECountdownTimerType::Lobby)
	{
		StopCountdownTimer(LobbyCountdownTimer);
		LobbyStatus = ELobbyStatus::SeamlessTravelling;
		TrySeamlessTravel(DestinationMap);
	}
}



void ADS_LobbyGameMode::InitGameLift()
{
	// 把数据存进GameInstanceSubsystem中
	if (UGameInstance* GameInstance = GetGameInstance(); IsValid(GameInstance)) // 这是什么语法
	{
		if (DSGameInstanceSubsystem = GameInstance->GetSubsystem<UDS_GameInstanceSubsystem>(); IsValid(DSGameInstanceSubsystem))
		{
			FServerParameters ServerParameters;
			SetServerParameters(ServerParameters);
			DSGameInstanceSubsystem->InitGameLift(ServerParameters);
		}
	}
}

// 设置游戏服务器需要的服务器参数
void ADS_LobbyGameMode::SetServerParameters(FServerParameters& OutServerParameters)
{
	//AuthToken returned from the "aws gamelift get-compute-auth-token" API. Note this will expire and require a new call to the API after 15 minutes.
	if (FParse::Value(FCommandLine::Get(), TEXT("-authtoken="), OutServerParameters.m_authToken))
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("AUTH_TOKEN: %s"), *OutServerParameters.m_authToken)
	}

	//The Host/compute-name of the GameLift Anywhere instance.
	if (FParse::Value(FCommandLine::Get(), TEXT("-hostid="), OutServerParameters.m_hostId))
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("HOST_ID: %s"), *OutServerParameters.m_hostId)
	}

	//The Anywhere Fleet ID.
	if (FParse::Value(FCommandLine::Get(), TEXT("-fleetid="), OutServerParameters.m_fleetId))
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("FLEET_ID: %s"), *OutServerParameters.m_fleetId)
	}

	//The WebSocket URL (GameLiftServiceSdkEndpoint).
	if (FParse::Value(FCommandLine::Get(), TEXT("-websocketurl="), OutServerParameters.m_webSocketUrl))
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("WEBSOCKET_URL: %s"), *OutServerParameters.m_webSocketUrl)
	}

	//The process ID of the GameLift Anywhere instance.
	OutServerParameters.m_processId = FString::Printf(TEXT("%d"), GetCurrentProcessId());
	UE_LOG(LogDedicatedServers, Log, TEXT("PROCESS_ID: %s"), *OutServerParameters.m_processId)
}
