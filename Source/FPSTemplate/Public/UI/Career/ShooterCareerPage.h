// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Portal/Dashboard/CareerPage.h"
#include "ShooterCareerPage.generated.h"

class USpecialElimData;
struct FDSRetrieveMatchStatsResponse;
/**
 * 
 */
UCLASS()
class FPSTEMPLATE_API UShooterCareerPage : public UCareerPage
{
	GENERATED_BODY()

public:
	virtual void OnRetrieveMatchStats(const FDSRetrieveMatchStatsResponse& RetrieveMatchStatsResponse) override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpecialElimData> SpecialElimData;  // 用于从BP中获取icon等数据


	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCareerAchievement> CareerAchievementClass;
};
