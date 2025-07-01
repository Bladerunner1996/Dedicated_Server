#pragma once

#include "DSTypes.generated.h"  // 用于反射

// Timer state
UENUM(BlueprintType)
enum class ECountdownTimerState: uint8
{
	NotStarted UMETA(DisplayName = "Not Started"), // UMETA(DisplayName = "Not Started") 用于在蓝图中显示的名称
	Started UMETA(DisplayName = "Started"),
	Paused UMETA(DisplayName = "Paused"),
	Stopped UMETA(DisplayName = "Stopped")
};

// Timer type
UENUM(BlueprintType)
enum class ECountdownTimerType: uint8
{
	Lobby UMETA(DisplayName = "Lobby Countdown"),
	PreMatch UMETA(DisplayName = "Pre-Match Countdown"),
	Match UMETA(DisplayName = "Match Countdown"),
	PostMatch UMETA(DisplayName = "Post-Match Countdown"),
	None UMETA(DisplayName = "None")
	
};

UENUM()
enum class EMatchStatus: uint8
{
	WaitingForPlayers,
	PreMatch,
	Match,
	PostMatch,
	SeamlessTravelling
};

UENUM()
enum class ELobbyStatus : uint8
{
	WaitingForPlayers,
	CountdownToSeamlessTravel,
	SeamlessTravelling
};

// 一个倒计时Timer。用于处理倒计时的状态、类型、时间、更新间隔等
USTRUCT(BlueprintType)
struct FCountdownTimerHandle
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	ECountdownTimerState State = ECountdownTimerState::NotStarted;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	ECountdownTimerType Type = ECountdownTimerType::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CountdownTime = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CountdownUpdateInterval = 1.0f;

	FTimerHandle TimerFinishedHandle{};  // Finished handle用于处理倒计时结束时的事件
	FTimerHandle TimerUpdateHandle{};  // Update handle用于处理time更新时的事件

	FTimerDelegate TimerFinishedDelegate{};
	FTimerDelegate TimerUpdateDelegate{};
};

// 重载==操作符，用于比较两个FCountdownTimerHandle对象的Type是否相等
inline bool operator==(const FCountdownTimerHandle& A, const FCountdownTimerHandle& B)
{
	return A.Type == B.Type;
};