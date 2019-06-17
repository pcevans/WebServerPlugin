// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "Engine.h"
#include "Networking.h"

#include "Connection.h"
#include "DataConversionLibrary.h"
#include "HTTPMessageLibrary.h"

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
	TMap<FString, FString> URLHTMLResponses;
	TMap<FString, bool> SubURLCaptureAllowed;
	TMap<FString, FString> MIMETypeOverrides;
public:
	UFUNCTION(BlueprintCallable, Category = "Web Server")
	void StartServer();

	UFUNCTION(BlueprintCallable, Category = "Web Server")
	void ShutdownServer();

	UFUNCTION(BlueprintCallable, Category = "Web Server")
	void RegisterCallbackAtURL(FString URL, const FOnRequestReceived& Callback, bool bCaptureSubDirectoryURLs);

	UFUNCTION(BlueprintCallable, Category = "Web Server")
	void RegisterHTMLAtURL(FString URL, FString HTML, bool bCaptureSubDirectoryURLs);

	UFUNCTION(BlueprintCallable, Category = "Web Server")
	void RegisterMIMETypeForExtension(FString Extension, FString MIMEType);

private:
	void ListenerSocketLoop();

	void ConnectionSocketLoop();

	FOnRequestReceived *GetRegisteredCallback(TArray<FString> SubDirectories);

	FString GetRegisteredHTMLResponse(TArray<FString> SubDirectories);
};
