// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dropdown.h"
#include "AccountDropdown.generated.h"

class UDSLocalPlayerSubsystem;
/**
 * 账号下拉框
 */
UCLASS()
class DEDICATEDSERVERS_API UAccountDropdown : public UDropdown
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

private:
	UDSLocalPlayerSubsystem* GetLocalPlayerSubsystem();
};
