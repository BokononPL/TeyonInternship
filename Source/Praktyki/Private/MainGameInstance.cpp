// Copyright 2025 Teyon. All Rights Reserved.


#include "MainGameInstance.h"


#include "CPPUtils.h"
#include "MainMenuGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UMainGameInstance::UMainGameInstance()
{
	SearchCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UMainGameInstance::SearchForSessionsComplete);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UMainGameInstance::OnJoinSessionComplete);
}

void UMainGameInstance::TravelToMap(FString MapName)
{
	if(AMainMenuGameMode* GameMode = Cast<AMainMenuGameMode>(GetWorld()->GetAuthGameMode()))
	{
		MaxLaps = GameMode->MaxLaps;
		ConnectedPlayersCount = GameMode->MainMenuControllers.Num();
	}
	UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), "servertravel " + MapName + "?listen", UGameplayStatics::GetPlayerController(GetWorld(),0));
}

void UMainGameInstance::QuitGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(),UGameplayStatics::GetPlayerController(GetWorld(),0), EQuitPreference::Quit, false);
}

void UMainGameInstance::HostGame(int32 MaxPlayers, bool ShouldUse_LAN)
{
	if(IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld()))
	{
		if(IOnlineSessionPtr OnlineSessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
			SessionSettings->bIsDedicated = false;
			SessionSettings->bIsLANMatch = ShouldUse_LAN;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->NumPublicConnections = MaxPlayers;
			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			if(OnlineSessionPtr->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), "xD", *SessionSettings))
			{
				Print("Session created", 20.0f);
				OnlineSessionPtr->StartSession("xD");
			}
			else
			{
				Print("Session not created", 20.0f);
			}
			
		}
	}
}

void UMainGameInstance::JoinGame(FSearchResult SearchResult)
{
	if(IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld()))
	{
		if(IOnlineSessionPtr OnlineSessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			OnJoinSessionCompleteDelegateHandle = OnlineSessionPtr->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			if(OnlineSessionPtr->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), "xD", SearchResult.SearchResult))
			{
				Print("Session joined", 20.0f);
			}
			else
			{
				Print("Session not joined", 20.0f);
			}
		}
	}
}

void UMainGameInstance::FindSessions(bool IsLanQuery, int32 MaxSearchResults)
{
	if(IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld()))
	{
		if(IOnlineSessionPtr OnlineSessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			SearchCompleteHandle = OnlineSessionPtr->AddOnFindSessionsCompleteDelegate_Handle(SearchCompleteDelegate);
			SearchSettings = MakeShareable(new FOnlineSessionSearch());
			SearchSettings->bIsLanQuery = IsLanQuery;
			SearchSettings->MaxSearchResults = MaxSearchResults;
			SearchSettings->TimeoutInSeconds = 10.0f;
			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			OnlineSessionPtr->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SearchSettings.ToSharedRef());
		}
	}
}

void UMainGameInstance::StartListenServer()
{
	UWorld* World = GetWorld();
	if (World)
	{
		FURL URL;
		URL.Map = World->GetMapName();
		World->Listen(URL);
		Print(URL.ToString(), 20.0f);
	}
}

void UMainGameInstance::SearchForSessionsComplete(bool IsSuccessful)
{
	if(IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld()))
	{
		if(IOnlineSessionPtr OnlineSessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			OnlineSessionPtr->ClearOnFindSessionsCompleteDelegate_Handle(SearchCompleteHandle);
			TArray<FSearchResult> Results;
			for(FOnlineSessionSearchResult SearchResult : SearchSettings->SearchResults)
			{
				Results.Add(FSearchResult(SearchResult));
			}
			OnSearchCompleted.Broadcast(Results);
			Print("Search Completed", 20.0f);
		}
	}
}

void UMainGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if(IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld()))
	{
		if(IOnlineSessionPtr OnlineSessionPtr = OnlineSubsystem->GetSessionInterface())
		{
			OnlineSessionPtr->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
			APlayerController * const PlayerController = GetFirstLocalPlayerController();
			FString TravelURL;

			if (PlayerController && OnlineSessionPtr->GetResolvedConnectString("xD", TravelURL))
			{
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}
