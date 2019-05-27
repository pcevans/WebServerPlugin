// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "Engine.h"
#include "Networking.h"
#include "Connection.h"
#include "UObject/NoExportTypes.h"
#include "WebServer.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRequestReceived, UConnection*, Connection);
/**
 * 
 */
UCLASS()
class AWebServer : public AActor
{
	GENERATED_BODY()
protected:
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

private:
	FSocket *ListenerSocket;
	TArray<FSocket*> ConnectionSockets;
	FTimerHandle ListenerTimer;
	FTimerHandle ConnectionTimer;

	TMap<FString, FOnRequestReceived> URLCallbacks;
	TMap<FString, bool> URLOverrides;
public:
	UFUNCTION(BlueprintCallable, Category = "Web Server")
	void StartServer();

	UFUNCTION(BlueprintCallable, Category = "Web Server")
	void ShutdownServer();

	UFUNCTION(BlueprintCallable, Category = "Web Server")
	void RegisterURL(FString URL, FString Verb, const FOnRequestReceived& Callback, bool bCaptureSubDirectoryURLs);

	//void RegisterURL(FString URI /*, callback*/);

private:
	void ListenerSocketLoop();

	void ConnectionSocketLoop();

	FString GetRegisteredURL(TArray<FString> SubDirectories);
};
