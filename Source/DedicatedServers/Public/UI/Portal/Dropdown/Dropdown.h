﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dropdown.generated.h"

class UWidgetSwitcher;
class UButton;
class UTextBlock;
class UImage;
/**
 * 通用的下拉框
 */
UCLASS()
class DEDICATEDSERVERS_API UDropdown : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> ExpandedWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> CollapsedWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Expander;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ButtonText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Triangle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateColor HoveredTextColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateColor UnHoveredTextColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Triangle_Up;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateBrush Triangle_Down;


protected:
	virtual void NativeConstruct() override;
	void SetStyleTransparent();
	virtual void NativePreConstruct() override;  // 在构造函数之前调用, 用于设置一些默认值

	UFUNCTION()
	void ToggleDropdown();  // toggle是切换的意思

	bool bIsExpanded;
	
	void Expand();
	void Collapse();
	
	UFUNCTION()
	void Hover();
	
	UFUNCTION()
	void UnHover();
};
