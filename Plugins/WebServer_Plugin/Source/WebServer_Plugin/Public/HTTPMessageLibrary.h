#pragma once

#include <string>
#include <regex>

#include "CoreMinimal.h"
#include "Engine.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "DataConversionLibrary.h"

#include "HTTPMessageLibrary.generated.h"

USTRUCT(BlueprintType)
struct FSHTTPRequestDetails
{
	GENERATED_BODY()

		//Request line
		//e.g. GET /Base/one/two/three.txt?var1=var1111&var2&var3=var3333 HTTP/1.1
		/////// FileURL = /Base/one/two/three.txt; BaseDirectory = /Base; SubDirectories = {/Base, /one, /two, /three.txt}
		///////	QueryParameters = {(var1,var1111), (var2,), (var3,var3333)}
public:
	UPROPERTY(BlueprintReadWrite)
		FString m_Verb;
	UPROPERTY(BlueprintReadWrite)
		FString m_FileURL;
	UPROPERTY(BlueprintReadWrite)
		FString m_BaseDirectory;
	UPROPERTY(BlueprintReadWrite)
		TArray<FString> m_SubDirectories;
	UPROPERTY(BlueprintReadWrite)
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

public:
	//Response line
	//e.g. HTTP/1.1 200 OK
	////// Code = 200
	UPROPERTY(BlueprintReadWrite)
		int32 m_Code;

	//Headers
	//e.g. Content-Type: text/html\n\n
	////// Headers = {(Content-Type,text/html)}
	UPROPERTY(BlueprintReadWrite)
		TMap<FString, FString> m_Headers;

	//Body
	//e.g. <!DOCTYPE html><html><body><h1>My First Web Server</h1><p>You have been served.</p></body></html>
	////// Body = {<,!,D,O,C,T,Y,P,E, ,h,t,m,l,>,<,h,t,m,l,>,<,b,o,d,y,>,<,h,1,>,M,y, ,F,i,r,s,t, ,W,e,b, ,S,e,r,v,e,r,<,/,h,1,>,<,p,>,Y,o,u, ,h,a,v,e, ,b,e,e,n, ,s,e,r,v,e,d,.,<,/,p,>,<,/,b,o,d,y,>,<,/,h,t,m,l,>}
	UPROPERTY(BlueprintReadWrite)
		TArray<uint8> m_Body;

	//Full response example:
	//HTTP/1.1 200 OK
	//Content-Type: text/html\n\n
	//
	//<!DOCTYPE html><html><body><h1>My First Web Server</h1><p>You have been served.</p></body></html>
};

UCLASS()
class WEBSERVER_PLUGIN_API UHTTPMessageLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Data Conversion")
		static FSHTTPRequestDetails ParseRawRequest(TArray<uint8> RequestBytes);
	UFUNCTION(BlueprintCallable, Category = "Data Conversion")
		static TArray<uint8> BuildRawResponse(FSHTTPResponseDetails ResponseStruct);
	UFUNCTION(BlueprintCallable, Category = "Data Conversion")
		static FString ConvertFileTypeToMIMEType(FString FileExtension);
	UFUNCTION(BlueprintCallable, Category = "Data Conversion")
		static FString ConvertStatusCodeToString(int32 StatusCode);

private:
	//For HTTP request string to struct
	static void ParseRequestLine(FString Line, FSHTTPRequestDetails &ParsedRequestDetails);
	static void ParseRequestHeaderLine(FString Line, FSHTTPRequestDetails &ParsedRequestDetails);
	static void ParseRequestBody(TArray<FString> RequestLines, FSHTTPRequestDetails &ParsedRequestDetails, int32 StartLine);

};