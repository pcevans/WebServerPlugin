// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DataConversionLibrary.generated.h"

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
	
};
