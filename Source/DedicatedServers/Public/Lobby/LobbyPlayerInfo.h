#pragma once

#include "Net/Serialization/FastArraySerializer.h"
#include "LobbyPlayerInfo.generated.h"

// 1 . Create a struct that inherits from FFastArraySerializerItem
USTRUCT(BlueprintType)
struct FLobbyPlayerInfo : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FLobbyPlayerInfo() {}
	FLobbyPlayerInfo(const FString& Name) : UserName(Name) {}

	UPROPERTY(BlueprintReadWrite)
	FString UserName{};
};

// 2 . Create a struct that inherits from FFastArraySerializer
USTRUCT()
struct FLobbyPlayerInfoArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FLobbyPlayerInfo> Players;

	// 3 . Implement the NetDeltaSerialize function
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FLobbyPlayerInfo, FLobbyPlayerInfoArray>(Players, DeltaParams, *this);
	}

	void AddPlayer(const FLobbyPlayerInfo& NewPlayerInfo);
	void RemovePlayer(const FString& Username);
};

// 4 . Implement the TStructOpsTypeTraits specialization。 模版特化，控制 UStruct的行为，例如序列化，网络同步，复制等
template<>
struct TStructOpsTypeTraits<FLobbyPlayerInfoArray> : public TStructOpsTypeTraitsBase2<FLobbyPlayerInfoArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
