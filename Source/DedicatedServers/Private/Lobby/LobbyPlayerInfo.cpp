#include "Lobby/LobbyPlayerInfo.h"

void FLobbyPlayerInfoArray::AddPlayer(const FLobbyPlayerInfo& NewPlayerInfo)
{
	int32 Index = Players.Add(NewPlayerInfo);
	MarkItemDirty(Players[Index]);  // 标记为脏数据，通知网络系统需要更新
	Players[Index].PostReplicatedAdd(*this);  // 调用添加后的处理函数
}

void FLobbyPlayerInfoArray::RemovePlayer(const FString& Username)
{
	for (int32 PlayerIndex = 0; PlayerIndex < Players.Num(); ++PlayerIndex)
	{
		if (Players[PlayerIndex].UserName == Username)
		{
			Players[PlayerIndex].PreReplicatedRemove(*this);  // 调用删除前的处理函数
			Players.RemoveAtSwap(PlayerIndex);  // 把最后一个元素移到删除位置。比RemoveAt高效
			MarkArrayDirty();
			break;
		}
	}
}
