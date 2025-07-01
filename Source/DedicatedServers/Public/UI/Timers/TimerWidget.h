// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/DSTypes.h"
#include "TimerWidget.generated.h"

class ADSPlayerController;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UTimerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
protected:

	// Widget有Timer的类型，别忘了设置
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	ECountdownTimerType TimerType = ECountdownTimerType::None;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ADSPlayerController> OwningPlayerController;

	UPROPERTY(BlueprintReadOnly)
	bool bActive = false;

	UPROPERTY(EditDefaultsOnly)
	bool bHiddenWhenInactive = true;

	UPROPERTY(EditDefaultsOnly)
	bool bShowCentiSeconds = false;
	
	UFUNCTION()
	virtual void OnTimerUpdated(float TimeLeft, ECountdownTimerType Type);

	UFUNCTION()
	virtual void OnTimerStopped(float TimeLeft, ECountdownTimerType Type);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Timer Updated"))
	void K2_OnTimerUpdated(float TimeLeft, ECountdownTimerType Type);  // K2_前缀表示这个函数是蓝图实现的

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Timer Started"))
	void K2_OnTimerStarted(float TimeLeft, ECountdownTimerType Type);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Timer Stopped"))
	void K2_OnTimerStopped(float TimeLeft, ECountdownTimerType Type);

	FString FormatTimeAsString(float TimeSeconds) const;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Time;

	float InternalCountdown;

	void TimerStarted(float InitialTime);
	void TimerStopped();
	void UpdateCountdown(float TimeSeconds);
};


