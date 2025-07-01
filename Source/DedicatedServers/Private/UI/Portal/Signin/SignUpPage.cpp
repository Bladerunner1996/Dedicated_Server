// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Signin/SignUpPage.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void USignUpPage::UpdateStatusMessage(const FString& Message, bool bShouldResetWidgets)
{
	TextBlock_StatusMessage->SetText(FText::FromString(Message));
	if (bShouldResetWidgets)
	{
		Button_SignUp->SetIsEnabled(true);
	}
}

void USignUpPage::ClearTextBoxes()
{
	TextBox_Username->SetText(FText::FromString(TEXT("")));
	TextBox_Password->SetText(FText::FromString(TEXT("")));
	TextBox_ConfirmPassword->SetText(FText::FromString(TEXT("")));
	TextBox_Email->SetText(FText::FromString(TEXT("")));
	TextBlock_StatusMessage->SetText(FText::FromString(TEXT("")));
}

void USignUpPage::NativeConstruct()
{
	Super::NativeConstruct();

	TextBox_Username->OnTextChanged.AddDynamic(this, &ThisClass::UpdateSignUpButtonState);
	TextBox_Password->OnTextChanged.AddDynamic(this, &ThisClass::UpdateSignUpButtonState);
	TextBox_ConfirmPassword->OnTextChanged.AddDynamic(this, &ThisClass::UpdateSignUpButtonState);
	TextBox_Email->OnTextChanged.AddDynamic(this, &ThisClass::UpdateSignUpButtonState);
	
	Button_SignUp->SetIsEnabled(false);
}

void USignUpPage::UpdateSignUpButtonState(const FText& Text)
{
	const bool bIsUsernameValid = !TextBox_Username->GetText().ToString().IsEmpty();
	FString Password = TextBox_Password->GetText().ToString();
	FString ConfirmPassword = TextBox_ConfirmPassword->GetText().ToString();
	const bool bArePasswordsEqual = Password.Equals(ConfirmPassword);
	const bool bIsEmailValid = IsValidEmail(TextBox_Email->GetText().ToString());
	
	FString StatusMessage;
	const bool bIsPasswordStrong = IsStrongPassword(TextBox_Password->GetText().ToString());

	if (!bIsUsernameValid)
	{
		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("用户名不能为空")));
	}
	else if (!bIsPasswordStrong && !TextBox_Password->GetText().ToString().IsEmpty())
	{
		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("密码长度至少为8位，且包含数字、大小写字母和特殊字符")));
	}
	else if (!bArePasswordsEqual && !TextBox_ConfirmPassword->GetText().ToString().IsEmpty())
	{
		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("两次输入的密码不一致")));
	}
	else if (!bIsEmailValid && !TextBox_Email->GetText().ToString().IsEmpty())
	{
		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("邮箱格式不正确")));
	}
	else
	{
		TextBlock_StatusMessage->SetText(FText::FromString(TEXT("")));
	}
	Button_SignUp->SetIsEnabled(bIsUsernameValid && bArePasswordsEqual && bIsEmailValid && bIsPasswordStrong);
}

bool USignUpPage::IsValidEmail(const FString& Email)
{
	const FRegexPattern EmailPattern(TEXT(R"((^[^\s@]+@[^\s@]+\.[^\s@]{2,}$))"));  //正则表达式Regular expression. R"" is a raw string literal.
	FRegexMatcher Matcher(EmailPattern, Email);
	return Matcher.FindNext();
}

bool USignUpPage::IsStrongPassword(const FString& Password)
{
	const FRegexPattern NumberPattern(TEXT(R"(\d)"));  // 至少有一个数字
	const FRegexPattern SpecialCharPattern(TEXT(R"([^\w\s])"));  // 至少有一个非a-Z非空白字符
	const FRegexPattern UpperCasePattern(TEXT(R"([A-Z])"));  // 至少有一个大写字母
	const FRegexPattern LowerCasePattern(TEXT(R"([a-z])"));  // 至少有一个小写字母

	FRegexMatcher NumberMatcher(NumberPattern, Password);
	FRegexMatcher SpecialCharMatcher(SpecialCharPattern, Password);
	FRegexMatcher UpperCaseMatcher(UpperCasePattern, Password);
	FRegexMatcher LowerCaseMatcher(LowerCasePattern, Password);
	
	const bool bIsPassLongEnough = TextBox_Password->GetText().ToString().Len() >= 8;
	if (!NumberMatcher.FindNext()
		|| !SpecialCharMatcher.FindNext()
		|| !UpperCaseMatcher.FindNext()
		|| !LowerCaseMatcher.FindNext()
		|| !bIsPassLongEnough)
	{
		return false;
	}
	return true;
}