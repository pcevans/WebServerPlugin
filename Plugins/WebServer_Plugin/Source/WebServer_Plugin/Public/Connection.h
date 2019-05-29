// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include <regex>

#include "Core.h"
#include "CoreUObject.h"
#include "Engine.h"
#include "Networking.h"

#include "DataConversionLibrary.h"

#include "Connection.generated.h"

USTRUCT(BlueprintType)
struct FSHTTPRequestDetails
{
	GENERATED_BODY()

	//Request line
	//e.g. GET /Base/one/two/three.txt?var1=var1111&var2&var3=var3333 HTTP/1.1
	/////// FileURL = /Base/one/two/three.txt; BaseDirectory = /Base; SubDirectories = {/Base, /one, /two, /three.txt}
	///////	QueryParameters = {(var1,var1111), (var2,), (var3,var3333)}
	UPARAM(BlueprintReadWrite)
	FString m_Verb;
	UPARAM(BlueprintReadWrite)
	FString m_FileURL;
	UPARAM(BlueprintReadWrite)
	FString m_BaseDirectory;
	UPARAM(BlueprintReadWrite)
	TArray<FString> m_SubDirectories;
	UPARAM(BlueprintReadWrite)
	TMap<FString, FString> m_QueryParameters;

	//Headers
	//e.g) Host: localhost:8080
	//	   Authorization: Basic xskjadsdjaoirjoiqj28u98r3uf8hdufhcds7fy7hq13oij
	//	   Accept: text/html
	////// Host = localhost:8080; Headers = {(Host,localhost:8080), (Authorization,Bearer xskjadsdjaoirjoiqj28u98r3uf8hdufhcds7fy7hq13oij), (Accept,text/html)}
	FString m_Host;
	TMap<FString, FString> m_Headers;

	//Body
	TArray<uint8> m_Body;

	//Full request example:
	//GET /Base/one/two/three.txt?var1=var1111&var2&var3=var3333 HTTP/1.1
	//Host: localhost:8080
	//Authorization: Basic xskjadsdjaoirjoiqj28u98r3uf8hdufhcds7fy7hq13oij
	//Accept: text/html
};

USTRUCT(BlueprintType)
struct FSHTTPResponseDetails
{
	GENERATED_BODY()

	//Response line
	//e.g. HTTP/1.1 200 OK
	////// Code = 200
	UPARAM(BlueprintReadWrite)
	int32 m_Code;

	//Headers
	//e.g. Content-Type: text/html\n\n
	////// Headers = {(Content-Type,text/html)}
	UPARAM(BlueprintReadWrite)
	TMap<FString, FString> m_Headers;

	//Body
	//e.g. <!DOCTYPE html><html><body><h1>My First Web Server</h1><p>You have been served.</p></body></html>
	////// Body = {<,!,D,O,C,T,Y,P,E, ,h,t,m,l,>,<,h,t,m,l,>,<,b,o,d,y,>,<,h,1,>,M,y, ,F,i,r,s,t, ,W,e,b, ,S,e,r,v,e,r,<,/,h,1,>,<,p,>,Y,o,u, ,h,a,v,e, ,b,e,e,n, ,s,e,r,v,e,d,.,<,/,p,>,<,/,b,o,d,y,>,<,/,h,t,m,l,>}
	UPARAM(BlueprintReadWrite)
	TArray<uint8> m_Body;

	//Full response example:
	//HTTP/1.1 200 OK
	//Content-Type: text/html\n\n
	//
	//<!DOCTYPE html><html><body><h1>My First Web Server</h1><p>You have been served.</p></body></html>
};


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

	void ParseRequestDetails(TArray<uint8> ReceivedData);
	void ParseRequestLine(FString Line, FSHTTPRequestDetails &ParsedRequestDetails);
	void ParseHeaderLine(FString Line, FSHTTPRequestDetails &ParsedRequestDetails);
	void ParseBody(TArray<FString> RequestLines, FSHTTPRequestDetails &ParsedRequestDetails, int32 StartLine);	
};
