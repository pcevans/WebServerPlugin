// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <string>
#include <regex>

#include "CoreMinimal.h"
#include "Engine.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "DataStructures.h"

#include "DataConversionLibrary.generated.h"

enum EJsonTypes {
	None,
	Object,
	Attribute,
	Array
};

/**
 * 
 */
UCLASS()
class WEBSERVER_PLUGIN_API UDataConversionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Data Conversion")
		static FString ConvertByteArrayToString(TArray<uint8> ByteArray);
	UFUNCTION(BlueprintCallable, Category = "Data Conversion")
		static TArray<uint8> ConvertStringToByteArray(FString String);
	UFUNCTION(BlueprintCallable, Category = "Data Conversion")
		static FString ConvertHTTPStatusCodeToString(int32 StatusCode);
	//UFUNCTION(BlueprintCallable, Category = "Data Conversion")
	//	static FSDataObject ConvertJSONToDataObject(FString JSON);
	//UFUNCTION(BlueprintCallable, Category = "Data Conversion")
	//	static FSDataObject ConvertXMLToDataObject(FString XML);
	UFUNCTION(BlueprintCallable, Category = "Data Conversion")
		static FSHTTPRequestDetails ConvertHTTPRequestBytesToDetailStruct(TArray<uint8> RequestBytes);
	UFUNCTION(BlueprintCallable, Category = "Data Conversion")
		static TArray<uint8> ConvertHTTPResponseDetailStructToBytes(FSHTTPResponseDetails ResponseStruct);


private:
	//For JSON to struct
	static void ParseJsonInternal(FSDataObject &DataObject, std::string JSON, EJsonTypes LastType);

	//For HTTP request string to struct
	static void ParseRequestLine(FString Line, FSHTTPRequestDetails &ParsedRequestDetails);
	static void ParseHeaderLine(FString Line, FSHTTPRequestDetails &ParsedRequestDetails);
	static void ParseBody(TArray<FString> RequestLines, FSHTTPRequestDetails &ParsedRequestDetails, int32 StartLine);
	
};
