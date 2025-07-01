// Fill out your copyright notice in the Description page of Project Settings.


#include "DedicatedServers/Public/Game/DS_GameMode.h"

DEFINE_LOG_CATEGORY(LogDS_GameMode); // 定义一个新的日志类别

void ADS_GameMode::BeginPlay()
{
	Super::BeginPlay();
#if WITH_GAMELIFT
	InitGameLift();
#endif
}

void ADS_GameMode::InitGameLift()
{
	UE_LOG(LogDS_GameMode, Log, TEXT("Initializing the GameLift Server"));

	//Getting the module first.
	FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK")); // 加载GameLiftServerSDK模块

	//Define the server parameters for a GameLift Anywhere fleet. These are not needed for a GameLift managed EC2 fleet.
	FServerParameters ServerParameters;

	SetServerParameters(ServerParameters);

	//InitSDK establishes a local connection with GameLift's agent to enable further communication.
	//Use InitSDK(serverParameters) for a GameLift Anywhere fleet. 
	//Use InitSDK() for a GameLift managed EC2 fleet.
	GameLiftSdkModule->InitSDK(ServerParameters);

	// lambda function
	auto OnGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
	{
		FString GameSessionId = FString(gameSession.GetGameSessionId());
		UE_LOG(LogDS_GameMode, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
		GameLiftSdkModule->ActivateGameSession();
	};

	ProcessParameters.OnStartGameSession.BindLambda(OnGameSession);

	auto OnProcessTerminate = [=]()
	{
		UE_LOG(LogDS_GameMode, Log, TEXT("Game Server Process is terminating"));
		GameLiftSdkModule->ProcessEnding();
	};

	ProcessParameters.OnTerminate.BindLambda(OnProcessTerminate);

	auto OnHealthCheck = []()
	{
		UE_LOG(LogDS_GameMode, Log, TEXT("Performing Health Check"));
		return true;
	};

	ProcessParameters.OnHealthCheck.BindLambda(OnHealthCheck); // 这个函数返回true表示健康检查通过，健康检查的目的是确保游戏服务器实例正常运行

	int32 Port = FURL::UrlConfig.DefaultPort; // 默认端口号是7777
	ParseCommandLinePort(Port); // 从命令行参数中解析端口号。Parse:解析

	ProcessParameters.port = Port;

	TArray<FString> LogFiles;
	LogFiles.Add(TEXT("FPSTemplate/Saved/Logs/FPSTemplate.log"));
	ProcessParameters.logParameters = LogFiles;  // 日志文件

	UE_LOG(LogDS_GameMode, Log, TEXT("Calling ProcessReady"));
	GameLiftSdkModule->ProcessReady(ProcessParameters);
}

// 设置游戏服务器需要的服务器参数
void ADS_GameMode::SetServerParameters(FServerParameters& OutServerParameters)
{
	//AuthToken returned from the "aws gamelift get-compute-auth-token" API. Note this will expire and require a new call to the API after 15 minutes.
	if (FParse::Value(FCommandLine::Get(), TEXT("-authtoken="), OutServerParameters.m_authToken))
	{
		UE_LOG(LogDS_GameMode, Log, TEXT("AUTH_TOKEN: %s"), *OutServerParameters.m_authToken)
	}

	//The Host/compute-name of the GameLift Anywhere instance.
	if (FParse::Value(FCommandLine::Get(), TEXT("-hostid="), OutServerParameters.m_hostId))
	{
		UE_LOG(LogDS_GameMode, Log, TEXT("HOST_ID: %s"), *OutServerParameters.m_hostId)
	}

	//The Anywhere Fleet ID.
	if (FParse::Value(FCommandLine::Get(), TEXT("-fleetid="), OutServerParameters.m_fleetId))
	{
		UE_LOG(LogDS_GameMode, Log, TEXT("FLEET_ID: %s"), *OutServerParameters.m_fleetId)
	}

	//The WebSocket URL (GameLiftServiceSdkEndpoint).
	if (FParse::Value(FCommandLine::Get(), TEXT("-websocketurl="), OutServerParameters.m_webSocketUrl))
	{
		UE_LOG(LogDS_GameMode, Log, TEXT("WEBSOCKET_URL: %s"), *OutServerParameters.m_webSocketUrl)
	}

	//The process ID of the GameLift Anywhere instance.
	OutServerParameters.m_processId = FString::Printf(TEXT("%d"), GetCurrentProcessId());
	UE_LOG(LogDS_GameMode, Log, TEXT("PROCESS_ID: %s"), *OutServerParameters.m_processId)
}

// 从命令行参数中解析端口号
void ADS_GameMode::ParseCommandLinePort(int32& OutPort)
{
	TArray<FString> CommandLineTokens;
	TArray<FString> CommandLineSwitches;
	FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);
	for (const FString& Switch : CommandLineSwitches)
	{
		FString Key;
		FString Value;
		if (Switch.Split(TEXT("="), &Key, &Value))
		{
			if (Key.Equals(TEXT("port"), ESearchCase::IgnoreCase))
			{
				OutPort = FCString::Atoi(*Value);
				return;
			}
		}
	}
}
