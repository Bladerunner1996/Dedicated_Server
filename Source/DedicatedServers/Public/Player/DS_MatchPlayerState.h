// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DS_MatchPlayerState.generated.h"

struct FDSRecordMatchStatsInput;
class UGameStatsManager;
/**
 * 如果没有这个中间层 DS_MatchPlayerState，那么记录stats时，就要知道具体的游戏中 MatchPlayerState的类型，那样这个DS module就不通用了。
 * 这里定义通用的 GameStatsManagerClass, GameStatsManager指针，OnMatchEnded空的虚函数。让游戏中的 MatchPlayerState 继承这个类，利用多态。
 * 运行时多态：通过基类指针或引用调用虚函数时，根据【实际对象类型】动态决定调用哪个派生类的实现，体现运行时多态
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API ADS_MatchPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void OnMatchEnded(const FString& Username);  //  在FPSTemplate/Public/Player/MatchPlayerState.h override, 用于结束match时记录玩家的match stats

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameStatsManager> GameStatsManagerClass;

protected:
	virtual void BeginPlay() override;
	void RecordMatchStats(const FDSRecordMatchStatsInput& RecordMatchStatsInput) const;

private:

	UPROPERTY()
	TObjectPtr<UGameStatsManager> GameStatsManager;
};
