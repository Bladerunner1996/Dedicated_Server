// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccountDropdown_Expanded.generated.h"

class UDSLocalPlayerSubsystem;
class UPortalManager;
class UButton;
class UTextBlock;
/**
 * 展开的账号下拉框
 */
UCLASS()
class DEDICATEDSERVERS_API UAccountDropdown_Expanded : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPortalManager> PortalManagerClass;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_SignOut;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_SignOutButtonText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Email;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateColor HoveredTextColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateColor UnHoveredTextColor;

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	UFUNCTION()
	void SignOutButton_Hover();
	
	UFUNCTION()
	void SignOutButton_UnHover();

	UFUNCTION()
	void SignOutButton_OnClicked();

private:
	UPROPERTY()
	TObjectPtr<UPortalManager> PortalManager;

	void SetSignOutButtonTransparent();

	UDSLocalPlayerSubsystem* GetLocalPlayerSubsystem();
};
