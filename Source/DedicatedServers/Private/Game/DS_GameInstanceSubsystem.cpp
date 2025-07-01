// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DS_GameInstanceSubsystem.h"

#include "DedicatedServers/DedicatedServers.h"

UDS_GameInstanceSubsystem::UDS_GameInstanceSubsystem()
{
	bGameLiftInitialized = false;
}

void UDS_GameInstanceSubsystem::InitGameLift(const FServerParameters& ServerParameters)
{
	if (bGameLiftInitialized) return;

#if WITH_GAMELIFT
	UE_LOG(LogDedicatedServers, Log, TEXT("Initializing the GameLift Server"));

	//Getting the module first.
	FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK")); // 加载GameLiftServerSDK模块
	//InitSDK establishes a local connection with GameLift's agent to enable further communication.
	//Use InitSDK(serverParameters) for a GameLift Anywhere fleet. 
	//Use InitSDK() for a GameLift managed EC2 fleet.
	GameLiftSdkModule->InitSDK(ServerParameters);
	// lambda function
	auto OnGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
	{
		FString GameSessionId = FString(gameSession.GetGameSessionId());
		UE_LOG(LogDedicatedServers, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
		GameLiftSdkModule->ActivateGameSession();
	};

	ProcessParameters.OnStartGameSession.BindLambda(OnGameSession);

	auto OnProcessTerminate = [=]()
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("Game Server Process is terminating"));
		GameLiftSdkModule->ProcessEnding();
	};

	ProcessParameters.OnTerminate.BindLambda(OnProcessTerminate);

	auto OnHealthCheck = []()
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("Performing Health Check"));
		return true;
	};

	ProcessParameters.OnHealthCheck.BindLambda(OnHealthCheck); // 这个函数返回true表示健康检查通过，健康检查的目的是确保游戏服务器实例正常运行

	int32 Port = FURL::UrlConfig.DefaultPort; // 默认端口号是7777
	ParseCommandLinePort(Port); // 从命令行参数中解析端口号。Parse:解析

	ProcessParameters.port = Port;

	TArray<FString> LogFiles;
	LogFiles.Add(TEXT("FPSTemplate/Saved/Logs/FPSTemplate.log"));
	ProcessParameters.logParameters = LogFiles;  // 日志文件

	UE_LOG(LogDedicatedServers, Log, TEXT("Calling ProcessReady"));
	GameLiftSdkModule->ProcessReady(ProcessParameters);
	
#endif
	bGameLiftInitialized = true;
}

void UDS_GameInstanceSubsystem::ParseCommandLinePort(int32& OutPort)
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
			if (Key.Equals(TEXT("port"), ESearchCase::IgnoreCase) && Value.IsNumeric())
			{
				LexFromString(OutPort, *Value); // LexFromString:将字符串转换为整数
				return;
			}
		}
	}
}
