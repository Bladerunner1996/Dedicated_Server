// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ListFleetsBox.generated.h"

class UScrollBox;
class UButton;
/**
 * 
 */
UCLASS()
class DEDICATEDSERVERS_API UListFleetsBox : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta = (BindWidget)) //绑定到蓝图中的ScrollBox, 从而可以在代码中访问BP中的ScrollBox
	TObjectPtr<UScrollBox> ScrollBox_ListFleets;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_ListFleets;
};
