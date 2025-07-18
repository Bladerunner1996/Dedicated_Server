﻿#include "UI/HTTP/HTTPRequestTypes.h"
#include "DedicatedServers/DedicatedServers.h"

void FDSMetaData::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("httpStatusCode: %d"), httpStatusCode);
	UE_LOG(LogDedicatedServers, Log, TEXT("requestId: %s"), *requestId);
	UE_LOG(LogDedicatedServers, Log, TEXT("attemps: %d"), attemps);
	UE_LOG(LogDedicatedServers, Log, TEXT("totalRetryDelay: %f"), totalRetryDelay);
}

void FDSListFleetsResponse::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("ListFleetsResponse:"));
	for (const FString& FleetId : FleetIds)
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("	FleetId: %s"), *FleetId);
	}
	if (!NextToken.IsEmpty())
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("	NextToken: %s"), *NextToken);
	}
}

void FDSGameSession::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("GameSession:"));
	UE_LOG(LogDedicatedServers, Log, TEXT("	CreationTime: %s"), *CreationTime);
	UE_LOG(LogDedicatedServers, Log, TEXT("	CreatorId: %s"), *CreatorId);
	UE_LOG(LogDedicatedServers, Log, TEXT("	CurrentPlayerSessionCount: %d"), CurrentPlayerSessionCount);
	UE_LOG(LogDedicatedServers, Log, TEXT("	DnsName: %s"), *DnsName);
	UE_LOG(LogDedicatedServers, Log, TEXT("	FleetArn: %s"), *FleetArn);
	UE_LOG(LogDedicatedServers, Log, TEXT("	FleetId: %s"), *FleetId);

	// Dump GameProperties (TMap)
	UE_LOG(LogDedicatedServers, Log, TEXT("GameProperties:"));
	for (const TPair<FString, FString>& Pair : GameProperties)
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("  Key: %s, Value: %s"), *Pair.Key, *Pair.Value);
	}

	UE_LOG(LogDedicatedServers, Log, TEXT("	GameSessionData: %s"), *GameSessionData);
	UE_LOG(LogDedicatedServers, Log, TEXT("	GameSessionId: %s"), *GameSessionId);
	UE_LOG(LogDedicatedServers, Log, TEXT("	IPAddress: %s"), *IPAddress);
	UE_LOG(LogDedicatedServers, Log, TEXT("	Location: %s"), *Location);
	UE_LOG(LogDedicatedServers, Log, TEXT("	MatchmakerData: %s"), *MatchmakerData);
	UE_LOG(LogDedicatedServers, Log, TEXT("	MaximumPlayerSessionCount: %d"), MaximumPlayerSessionCount);
	UE_LOG(LogDedicatedServers, Log, TEXT("	Name: %s"), *Name);
	UE_LOG(LogDedicatedServers, Log, TEXT("	PlayerSessionCreationPolicy: %s"), *PlayerSessionCreationPolicy);
	UE_LOG(LogDedicatedServers, Log, TEXT("	Port: %d"), Port);
	UE_LOG(LogDedicatedServers, Log, TEXT("	Status: %s"), *Status);
	UE_LOG(LogDedicatedServers, Log, TEXT("	StatusReason: %s"), *StatusReason);
	UE_LOG(LogDedicatedServers, Log, TEXT("	TerminationTime: %s"), *TerminationTime);
}

void FDSPlayerSession::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("PlayerSession:"));
	UE_LOG(LogDedicatedServers, Log, TEXT("	CreationTime: %s"), *CreationTime);
	UE_LOG(LogDedicatedServers, Log, TEXT("	DnsName: %s"), *DnsName);
	UE_LOG(LogDedicatedServers, Log, TEXT("	FleetId: %s"), *FleetId);
	UE_LOG(LogDedicatedServers, Log, TEXT("	GameSessionId: %s"), *GameSessionId);
	UE_LOG(LogDedicatedServers, Log, TEXT("	IpAddress: %s"), *IpAddress);
	UE_LOG(LogDedicatedServers, Log, TEXT("	PlayerData: %s"), *PlayerData);
	UE_LOG(LogDedicatedServers, Log, TEXT("	PlayerId: %s"), *PlayerId);
	UE_LOG(LogDedicatedServers, Log, TEXT("	PlayerSessionId: %s"), *PlayerSessionId);
	UE_LOG(LogDedicatedServers, Log, TEXT("	Port: %d"), Port);
	UE_LOG(LogDedicatedServers, Log, TEXT("	Status: %s"), *Status);
	UE_LOG(LogDedicatedServers, Log, TEXT("	TerminationTime: %s"), *TerminationTime);
}

void FDSCodeDeliveryDetails::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("CodeDeliveryDetails:"));
	UE_LOG(LogDedicatedServers, Log, TEXT("	AttributeName: %s"), *AttributeName);
	UE_LOG(LogDedicatedServers, Log, TEXT("	DeliveryMedium: %s"), *DeliveryMedium);
	UE_LOG(LogDedicatedServers, Log, TEXT("	Destination: %s"), *Destination);
}

void FDSSignUpResponse::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("FDSSignUpResponse Details:"));
	CodeDeliveryDetails.Dump();
	UE_LOG(LogDedicatedServers, Log, TEXT("	UserConfirmed: %s"), UserConfirmed ? TEXT("true") : TEXT("false"));
	UE_LOG(LogDedicatedServers, Log, TEXT("	UserSub: %s"), *UserSub);
}

void FDSNewDeviceMetaData::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("FDSNewDeviceMetaData:"));
	UE_LOG(LogDedicatedServers, Log, TEXT("	DeviceGroupKey: %s"), *DeviceGroupKey);
	UE_LOG(LogDedicatedServers, Log, TEXT("	DeviceKey: %s"), *DeviceKey);
}

void FDSAuthenticationResult::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("FDSAuthenticationResult:"));
	UE_LOG(LogDedicatedServers, Log, TEXT("	AccessToken: %s"), *AccessToken);
	UE_LOG(LogDedicatedServers, Log, TEXT("	ExpiresIn: %d"), ExpiresIn);
	UE_LOG(LogDedicatedServers, Log, TEXT("	IdToken: %s"), *IdToken);
	NewDeviceMetadata.Dump();
	UE_LOG(LogDedicatedServers, Log, TEXT("	RefreshToken: %s"), *RefreshToken);
	UE_LOG(LogDedicatedServers, Log, TEXT("	TokenType: %s"), *TokenType);
}

void FDSChallengeParameters::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("FDSChallengeParameters:"));
	for (const TPair<FString, FString>& Pair : Parameters)
	{
		UE_LOG(LogDedicatedServers, Log, TEXT("   %s : %s"), *Pair.Key, *Pair.Value);
	}
}

void FDSInitiateAuthResponse::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("FDSInitiateAuthResponse:"));
	AuthenticationResult.Dump();
	UE_LOG(LogDedicatedServers, Log, TEXT("	ChallengeName: %s"), *ChallengeName);
	ChallengeParameters.Dump();
	UE_LOG(LogDedicatedServers, Log, TEXT("	Session: %s"), *Session);
	UE_LOG(LogDedicatedServers, Log, TEXT("	email: %s"), *email);
}

void FDSRetrieveMatchStatsResponse::Dump() const
{
	UE_LOG(LogDedicatedServers, Log, TEXT("highestStreak: %d"), highestStreak);
	UE_LOG(LogDedicatedServers, Log, TEXT("dethroneElims: %d"), dethroneElims);
	UE_LOG(LogDedicatedServers, Log, TEXT("gotFirstBlood: %d"), gotFirstBlood);
	UE_LOG(LogDedicatedServers, Log, TEXT("defeats: %d"), defeats);
	UE_LOG(LogDedicatedServers, Log, TEXT("email: %s"), *email);
	UE_LOG(LogDedicatedServers, Log, TEXT("scoreElims: %d"), scoredElims);
	UE_LOG(LogDedicatedServers, Log, TEXT("headshotElims: %d"), headShotElims);
	UE_LOG(LogDedicatedServers, Log, TEXT("misses: %d"), misses);
	UE_LOG(LogDedicatedServers, Log, TEXT("revengeElims: %d"), revengeElims);
	UE_LOG(LogDedicatedServers, Log, TEXT("matchLosses: %d"), matchLosses);
	UE_LOG(LogDedicatedServers, Log, TEXT("showstopperElims: %d"), showstopperElims);
	UE_LOG(LogDedicatedServers, Log, TEXT("databaseid: %s"), *databaseid);
	UE_LOG(LogDedicatedServers, Log, TEXT("username: %s"), *username);
	UE_LOG(LogDedicatedServers, Log, TEXT("hits: %d"), hits);
	UE_LOG(LogDedicatedServers, Log, TEXT("matchWins: %d"), matchWins);
}


