#pragma once

#include "HTTPRequestTypes.generated.h"  // 为了支持序列化，需要包含这个头文件

/**
 * 这些结构体用于解析HTTP请求的response json数据，存储
 */
USTRUCT()
struct FDSMetaData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 httpStatusCode{};  // 需要初始化   并且名字要和json中的key一致

	UPROPERTY()
	FString requestId{};
	
	UPROPERTY()
	int32 attemps{};

	UPROPERTY()
	double totalRetryDelay{};

	void Dump() const;
};

USTRUCT()
struct FDSListFleetsResponse
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FString> FleetIds;

	UPROPERTY()
	FString NextToken;

	void Dump() const;
};

USTRUCT()
struct FDSGameSession
{
	GENERATED_BODY()

	UPROPERTY()
	FString CreationTime{};

	UPROPERTY()
	FString CreatorId{};

	UPROPERTY()
	int32 CurrentPlayerSessionCount{};

	UPROPERTY()
	FString DnsName{};

	UPROPERTY()
	FString FleetArn{};
	
	UPROPERTY()
	FString FleetId{};

	UPROPERTY()
	TMap<FString, FString> GameProperties{};

	UPROPERTY()
	FString GameSessionData{};

	UPROPERTY()
	FString GameSessionId{};

	UPROPERTY()
	FString IPAddress{};

	UPROPERTY()
	FString Location{};

	UPROPERTY()
	FString MatchmakerData{};

	UPROPERTY()
	int32 MaximumPlayerSessionCount{};

	UPROPERTY()
	FString Name{};

	UPROPERTY()
	FString PlayerSessionCreationPolicy{};

	UPROPERTY()
	int32 Port{};

	UPROPERTY()
	FString Status{};

	UPROPERTY()
	FString StatusReason{};

	UPROPERTY()
	FString TerminationTime{};

	void Dump() const;
};

USTRUCT()
struct FDSPlayerSession
{
	GENERATED_BODY()

	UPROPERTY()
	FString CreationTime{};

	UPROPERTY()
	FString DnsName{};

	UPROPERTY()
	FString FleetId{};

	UPROPERTY()
	FString FleetArn{};

	UPROPERTY()
	FString GameSessionId{};

	UPROPERTY()
	FString IpAddress{};

	UPROPERTY()
	FString PlayerData{};

	UPROPERTY()
	FString PlayerId{};

	UPROPERTY()
	FString PlayerSessionId{};

	UPROPERTY()
	int32 Port{};

	UPROPERTY()
	FString Status{};

	UPROPERTY()
	FString TerminationTime{};

	void Dump() const;
	
};

USTRUCT()
struct FDSCodeDeliveryDetails
{
	GENERATED_BODY()

	UPROPERTY()
	FString AttributeName;

	UPROPERTY()
	FString DeliveryMedium;

	UPROPERTY()
	FString Destination;  // email address

	void Dump() const;

};

USTRUCT()
struct FDSSignUpResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FDSCodeDeliveryDetails CodeDeliveryDetails;

	UPROPERTY()
	bool UserConfirmed;

	UPROPERTY()
	FString UserSub;

	void Dump() const;
};

USTRUCT()
struct FDSNewDeviceMetaData
{
	GENERATED_BODY()

	UPROPERTY()
	FString DeviceGroupKey;

	UPROPERTY()
	FString DeviceKey;

	void Dump() const;
};

USTRUCT()
struct FDSAuthenticationResult
{
	GENERATED_BODY()

	UPROPERTY()
	FString AccessToken;

	UPROPERTY()
	int32 ExpiresIn;

	UPROPERTY()
	FString IdToken;

	UPROPERTY()
	FDSNewDeviceMetaData NewDeviceMetadata;

	UPROPERTY()
	FString RefreshToken;

	UPROPERTY()
	FString TokenType;

	void Dump() const;
};

USTRUCT()
struct FDSChallengeParameters
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, FString> Parameters;

	void Dump() const;
};

USTRUCT()
struct FDSInitiateAuthResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FDSAuthenticationResult AuthenticationResult{};

	UPROPERTY()
	FString ChallengeName{};

	UPROPERTY()
	FDSChallengeParameters ChallengeParameters{};

	UPROPERTY()
	FString Session{};

	UPROPERTY()
	FString email{};  // 这个字段在sign in lambda中自己添加的，用于存储email

	void Dump() const;
};

// struct for json中的 matchstats。转换成json object时会变 camel case，所以这里定义也是camel case
USTRUCT()
struct FDSMatchStats
{
	GENERATED_BODY()

	UPROPERTY()
	int32 scoredElims{};

	UPROPERTY()
	int32 defeats{};

	UPROPERTY()
	int32 hits{};

	UPROPERTY()
	int32 misses{};

	UPROPERTY()
	int32 headShotElims{};

	UPROPERTY()
	int32 highestStreak{};

	UPROPERTY()
	int32 revengeElims{};

	UPROPERTY()
	int32 dethroneElims{};

	UPROPERTY()
	int32 showstopperElims{};

	UPROPERTY()
	int32 gotFirstBlood{};

	UPROPERTY()
	int32 matchWins{};

	UPROPERTY()
	int32 matchLosses{};
};

// struct for the input of the lambda
USTRUCT()
struct FDSRecordMatchStatsInput
{
	GENERATED_BODY()

	UPROPERTY()
	FDSMatchStats matchStats{};

	UPROPERTY()
	FString username{};
};

// 从DB中获取的match stats
USTRUCT()
struct FDSRetrieveMatchStatsResponse
{
	GENERATED_BODY()

	UPROPERTY()
	int32 highestStreak{};

	UPROPERTY()
	int32 dethroneElims{};

	UPROPERTY()
	int32 gotFirstBlood{};

	UPROPERTY()
	int32 defeats{};

	UPROPERTY()
	int32 scoredElims{};

	UPROPERTY()
	FString email{};

	UPROPERTY()
	int32 misses{};

	UPROPERTY()
	int32 revengeElims{};

	UPROPERTY()
	int32 matchLosses{};

	UPROPERTY()
	int32 showstopperElims{};

	UPROPERTY()
	FString databaseid{};

	UPROPERTY()
	int32 headShotElims{};

	UPROPERTY()
	FString username{};

	UPROPERTY()
	int32 hits{};

	UPROPERTY()
	int32 matchWins{};

	void Dump() const;
	
};

USTRUCT()
struct FDSLeaderboardItem
{
	GENERATED_BODY()

	UPROPERTY()
	FString databaseid{};
	
	UPROPERTY()
	FString username{};

	UPROPERTY()
	int32 matchWins{};

	UPROPERTY()
	int32 place{};
};
