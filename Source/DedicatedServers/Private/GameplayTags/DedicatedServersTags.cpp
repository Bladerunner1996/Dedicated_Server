#include "GameplayTags/DedicatedServersTags.h"

namespace DedicatedServersTags
{
	namespace GameSessionsAPI
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ListFleets, "DedicatedServers.GameSessionsAPI.ListFleets", "List Fleets resource on the GameSessions API");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FindOrCreateGameSession, "DedicatedServers.GameSessionsAPI.FindOrCreateGameSession", "Find or create a GameSession resource on the GameSessions API");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(CreatePlayerSession, "DedicatedServers.GameSessionsAPI.CreatePlayerSession", "Create a new PlayerSession on the GameSessions API");
	}
	namespace PortalAPI
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SignUp, "DedicatedServers.PortalAPI.SignUp", "Create a new User in the Portal API");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ConfirmSignUp, "DedicatedServers.PortalAPI.ConfirmSignUp", "Confirm a new User in Portal API");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SignIn, "DedicatedServers.PortalAPI.SignIn", "Retrieves Access Token, Id Token and Refresh Token in Portal API");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(SignOut, "DedicatedServers.PortalAPI.SignOut", "Signs user out and invalidates tokens in Portal API");
	}

	namespace GameStatsAPI
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(RecordMatchStats, "DedicatedServers.GameStatsAPI.RecordMatchStats", "Record all the stats of a match to DynamoDB in Game Stats API");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(RetrieveMatchStats, "DedicatedServers.GameStatsAPI.RetrieveMatchStats", "Retrieve the user's stats of a match from the Players table of DynamoDB");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(UpdateLeaderboard, "DedicatedServers.GameStatsAPI.UpdateLeaderboard", "Update the leaderboard with the user's stats in Game Stats API");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(RetrieveLeaderboard, "DedicatedServers.GameStatsAPI.RetrieveLeaderboard", "Retrieve the leaderboard from the DynamoDB in Game Stats API");
	}
}