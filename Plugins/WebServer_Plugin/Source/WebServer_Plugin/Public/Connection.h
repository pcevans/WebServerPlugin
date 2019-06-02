// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include <regex>

#include "Core.h"
#include "CoreUObject.h"
#include "Engine.h"
#include "Networking.h"

#include "DataConversionLibrary.h"
#include "DataStructures.h"

#include "Connection.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UConnection : public UObject
{
	GENERATED_BODY()

private:
	FSocket *ConnectionSocket;

	FSHTTPRequestDetails RequestDetails;
	FSHTTPResponseDetails ResponseDetails;

public:
	void Initialize(FSocket *ConnectionSocket, TArray<uint8> ReceivedData);

	UFUNCTION(BlueprintCallable, Category = "Web Server Response")
		void SendResponse();

	UFUNCTION(BlueprintCallable, Category = "Web Server Response")
		void SendResponseWithDetails(FSHTTPResponseDetails ResponseDetails);

	UFUNCTION(BlueprintCallable, Category = "Web Server Response")
		void SendSimpleHTMLResponse(int32 ResponseCode, FString HTML);

	UFUNCTION(BlueprintCallable, Category = "Web Server Response")
		void SetResponseCode(int32 Code) { ResponseDetails.m_Code = Code; }

	UFUNCTION(BlueprintCallable, Category = "Web Server Response")
		void SetResponseHeader(FString HeaderKey, FString HeaderValue) { ResponseDetails.m_Headers.Add(HeaderKey, HeaderValue); }

	UFUNCTION(BlueprintCallable, Category = "Web Server Response")
		void SetResponseHeaders(TMap<FString, FString> Headers) { ResponseDetails.m_Headers = Headers; }

	UFUNCTION(BlueprintCallable, Category = "Web Server Response")
		void SetResponseBody(TArray<uint8> Body) { ResponseDetails.m_Body = Body; }

	UFUNCTION(BlueprintCallable, Category = "Web Server Response")
		void SetResponseDetails(FSHTTPResponseDetails Details) { ResponseDetails = Details; }

	UFUNCTION(BlueprintPure, Category = "Web Server Request")
		FString GetRequestHeaderValue(FString Header) { return RequestDetails.m_Headers[Header]; }

	UFUNCTION(BlueprintPure, Category = "Web Server Request")
		TMap<FString, FString> GetRequestHeaders() { return RequestDetails.m_Headers; }

	UFUNCTION(BlueprintPure, Category = "Web Server Request")
		FString GetRequestQueryValue(FString Parameter) { return RequestDetails.m_QueryParameters[Parameter]; }

	UFUNCTION(BlueprintPure, Category = "Web Server Request")
		TMap<FString, FString> GetRequestQueryParameters() { return RequestDetails.m_QueryParameters; }

	UFUNCTION(BlueprintPure, Category = "Web Server Request")
		FString GetRequestHost() { return RequestDetails.m_Host; }

	UFUNCTION(BlueprintPure, Category = "Web Server Request")
		FString GetRequestFileURL() { return RequestDetails.m_FileURL; }

	UFUNCTION(BlueprintPure,Category = "Web Server Request")
		TArray<uint8> GetRequestBody() { return RequestDetails.m_Body; }
	
	UFUNCTION(BlueprintPure, Category = "Web Server Request")
		FString GetRequestBaseDirectory() { return RequestDetails.m_BaseDirectory; }

	UFUNCTION(BlueprintPure, Category = "Web Server Request")
		TArray<FString> GetRequestSubDirectories() { return RequestDetails.m_SubDirectories; }

	UFUNCTION(BlueprintPure, Category = "Web Server Request")
		FSHTTPRequestDetails GetRequestDetails() { return RequestDetails; }

private:
	void CreateAndSendMessage();
	FString CreateResponseMessage();

	
};
