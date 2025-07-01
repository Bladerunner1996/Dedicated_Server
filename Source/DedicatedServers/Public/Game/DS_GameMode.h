// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DS_GameModeBase.h"
#include "GameLiftServerSDK.h" // 1. 包含GameLiftServerSDK头文件
#include "DS_GameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDS_GameMode, Log, All); //2. 声明一个新的日志类别
/**
 * GameLift相关的功能。注意继承的类是DS_GameModeBase，意味着也有timer的功能..(已废弃，留着可以参考Gamelift流程）
 */
UCLASS()
class DEDICATEDSERVERS_API ADS_GameMode : public ADS_GameModeBase
{
	GENERATED_BODY()
public:

protected:
	virtual void BeginPlay() override;
private:

	FProcessParameters ProcessParameters; // 3. 定义一个ProcessParameters对象，这个对象需要在app生命周期一直存在

	void InitGameLift();
	void SetServerParameters(FServerParameters& OutServerParameters);
	void ParseCommandLinePort(int32& OutPort);
};
