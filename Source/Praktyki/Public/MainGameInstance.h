// Copyright 2025 Teyon. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PRAKTYKI_API FSearchResult
{
	GENERATED_BODY()

public:
	FSearchResult(){}
	FSearchResult(FOnlineSessionSearchResult SearchResult)
	{
		Ping = SearchResult.PingInMs;
		MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
		ConnectedPlayers = SearchResult.Session.SessionSettings.NumPublicConnections - SearchResult.Session.NumOpenPublicConnections;
		HostName = SearchResult.Session.OwningUserName;
		this->SearchResult = SearchResult;
	}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 ConnectedPlayers;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxPlayers;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Ping;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString HostName;

	FOnlineSessionSearchResult SearchResult;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSearchCompleted, const TArray<FSearchResult>&, SearchResults);

UCLASS()
class PRAKTYKI_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	
	UMainGameInstance();

	UFUNCTION(BlueprintCallable)
	void TravelToMap(FString MapName);

	UFUNCTION(BlueprintCallable)
	void QuitGame();

	UFUNCTION(BlueprintCallable)
	void HostGame(int32 MaxPlayers, bool ShouldUse_LAN);

	UFUNCTION(BlueprintCallable)
	void JoinGame(FSearchResult SearchResult);

	UFUNCTION(BlueprintCallable)
	void FindGames(bool IsLanQuery, int32 MaxSearchResults);

	UFUNCTION(BlueprintCallable)
	void StartListenServer();

	UFUNCTION(BlueprintCallable)
	void LeaveGame();

	UFUNCTION(BlueprintCallable)
	void DestroyGame();

	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FOnSearchCompleted OnSearchCompleted;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxLaps;

	UPROPERTY(BlueprintReadOnly)
	int32 ConnectedPlayersCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString MainMenuMapName;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayersCount;

	UPROPERTY(BlueprintReadOnly)
	bool ShouldFillWithBots;

	UPROPERTY(BlueprintReadOnly)
	int32 TimeLimit;

private:
	TSharedPtr<FOnlineSessionSearch> SearchSettings;
	FOnFindSessionsCompleteDelegate SearchCompleteDelegate;
	FDelegateHandle SearchCompleteHandle;
	void SearchForSessionsComplete(bool IsSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
};
