// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Timers/TimerWidget.h"

#include "Components/TextBlock.h"
#include "Kismet/KismetStringLibrary.h"
#include "Player/DSPlayerController.h"

void UTimerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OwningPlayerController = Cast<ADSPlayerController>(GetOwningPlayer());
	if (IsValid(OwningPlayerController))
	{
		OwningPlayerController->OnTimerUpdated.AddDynamic(this, &UTimerWidget::OnTimerUpdated);
		OwningPlayerController->OnTimerStopped.AddDynamic(this, &UTimerWidget::OnTimerStopped);
	}
	if (bHiddenWhenInactive)
	{
		TextBlock_Time->SetRenderOpacity(0.f);
	}
}

void UTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bActive)
	{
		UpdateCountdown(InternalCountdown - InDeltaTime);
	}
}

void UTimerWidget::OnTimerUpdated(float TimeLeft, ECountdownTimerType Type)
{
	if (Type!=TimerType) return;

	if (!bActive)
	{
		TimerStarted(TimeLeft);
	}
	UpdateCountdown(TimeLeft);
	K2_OnTimerUpdated(TimeLeft, TimerType);
	
}

void UTimerWidget::OnTimerStopped(float TimeLeft, ECountdownTimerType Type)
{
	if (Type!=TimerType) return;
	TimerStopped();
	K2_OnTimerStopped(TimeLeft, TimerType);
}

FString UTimerWidget::FormatTimeAsString(float TimeSeconds) const
{
	if (bShowCentiSeconds)
	{
		return UKismetStringLibrary::TimeSecondsToString(TimeSeconds);
	}
	else
	{
		const TCHAR* NegativeModifier = TimeSeconds < 0.f ? TEXT("-") : TEXT("");
		const int32 NumMinutes = FMath::FloorToInt(TimeSeconds/60.f);
		const int32 NumSeconds = FMath::FloorToInt(TimeSeconds-(NumMinutes*60.f));
		return FString::Printf(TEXT("%s%02d:%02d"), NegativeModifier, NumMinutes, NumSeconds);
	}
	
}

void UTimerWidget::TimerStarted(float InitialTime)
{
	bActive = true;
	TextBlock_Time->SetRenderOpacity(1.f);
	K2_OnTimerStarted(InitialTime, TimerType);
}

void UTimerWidget::TimerStopped()
{
	bActive = false;
	UpdateCountdown(0.f);
	if (bHiddenWhenInactive)
	{
		TextBlock_Time->SetRenderOpacity(0.f);
	}
	
}

void UTimerWidget::UpdateCountdown(float TimeSeconds)
{
	InternalCountdown = TimeSeconds;
	const FText TimeText = FText::FromString(FormatTimeAsString(InternalCountdown));
	TextBlock_Time->SetText(TimeText);
}
