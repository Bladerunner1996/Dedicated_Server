// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Signin/ConfirmSignUpPage.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void UConfirmSignUpPage::ClearTextBoxes()
{
	TextBox_ConfirmationCode->SetText(FText::FromString(TEXT("")));
	TextBlock_StatusMessage->SetText(FText::FromString(TEXT("")));
	TextBlock_Destination->SetText(FText::FromString(TEXT("")));
}

void UConfirmSignUpPage::UpdateStatusMessage(const FString& Message, bool bShouldResetWidgets)
{
	TextBlock_StatusMessage->SetText(FText::FromString(Message));
	if (bShouldResetWidgets)
	{
		Button_Confirm->SetIsEnabled(true);
	}
}

void UConfirmSignUpPage::NativeConstruct()
{
	Super::NativeConstruct();

	TextBox_ConfirmationCode->OnTextChanged.AddDynamic(this, &ThisClass::UpdateConfirmButtonState);
	Button_Confirm->SetIsEnabled(false);
}

// 强制要求输入6位数字验证码
void UConfirmSignUpPage::UpdateConfirmButtonState(const FText& Text)
{
	const FRegexPattern SixDigitsPattern(TEXT(R"(^\d{6}$)"));
	FRegexMatcher Matcher(SixDigitsPattern, Text.ToString());

	const bool bValidConfirmationCode = Matcher.FindNext();

	Button_Confirm->SetIsEnabled(bValidConfirmationCode);
	if (bValidConfirmationCode)
	{
		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("")));
	}
	else
	{
		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("请输入6位数字验证码")));
	}
	
}
