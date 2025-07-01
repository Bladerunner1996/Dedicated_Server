// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameLiftServerSDK.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DS_GameInstanceSubsystem.generated.h"

struct FServerParameters;
/**
 * 存储travel时的persistent数据（因为gamemode会被销毁）.负责GameLift的初始化和数据的存储
 */
UCLASS()
class DEDICATEDSERVERS_API UDS_GameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UDS_GameInstanceSubsystem();
	void InitGameLift(const FServerParameters& ServerParameters);

	UPROPERTY(BlueprintReadOnly)
	bool bGameLiftInitialized; // 是否已经初始化过GameLift

private:
	FProcessParameters ProcessParameters;// 定义一个ProcessParameters对象，这个对象需要在app生命周期一直存在

	void ParseCommandLinePort(int32& OutPort);// 从命令行参数中解析端口号
};
